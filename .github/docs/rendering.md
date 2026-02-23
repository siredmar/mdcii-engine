# Rendering System

The rendering system handles isometric projection, layer ordering, and depth sorting for the game view.

## Isometric Projection

### Grid to Screen Transformation

Anno 1602 uses a diamond-shaped isometric projection. Each tile is a 64×32 pixel diamond:

```
        /\
       /  \  ← 32 pixels high
      /    \
     /      \
    \       /
     \     /
      \   /
       \ /
        64 pixels wide
```

**Transformation formula:**

```go
// pkg/ecs/systems/renderer.go
const (
    TILE_WIDTH  = 64
    TILE_HEIGHT = 32
)

// Local coords (within island) to screen position
// Origin is the TOP POINT of the isometric diamond
originX := ((float64(x) - float64(y))) * (tileWidth / 2) + island.X * (tileWidth / 2)
originY := ((float64(x) + float64(y))) * (tileHeight / 2) + island.Y * (tileHeight / 2)
```

**Visual coordinate mapping:**

```
Grid:            Screen:
(0,0) (1,0)         (0,0)
(0,1) (1,1)      (-1,0.5) (1,0.5)
                    (0,1)
                 (-2,1.5) (0,1.5) (2,1.5)
```

### The Origin Point

The **origin** is the **top point** of each tile's diamond shape:

```
        ★ ← Origin point (originX, originY)
       /\
      /  \
     /    \
    /      \
   \       /
    \     /
     \   /
      \/
```

## Sprite Positioning with Pivots

Sprites are positioned using pivot points that define which pixel of the sprite aligns with the tile's origin:

```go
drawX := originX - float64(tile.PivotX)
drawY := originY - float64(tile.PivotY)
screen.DrawImage(tile.Image, op)
```

### Pivot Point Convention

The pivot is computed during atlas generation:
- Anchor = back corner of building at local (0,0)
- Pivot = offset from cropped image top-left to anchor

```
  Cropped sprite bounds
  ┌────────────────────┐
  │   ↓ pivotY         │
  │ ★─────┐            │  ★ = anchor (where origin aligns)
  │   ←   │            │
  │ pivotX             │
  │    ███████         │
  │   ██████████       │
  │  █████████████     │
  └────────────────────┘

drawX = originX - pivotX
drawY = originY - pivotY
```

### Why Back Corner Anchor?

Anno 1602 stores building positions at the **upper-left** grid cell (in original coordinates). When rendered isometrically, this maps to the **back corner** of the building footprint:

```
Grid view (3x3 building):    Isometric view:
(0,0) (1,0) (2,0)                    (0,0)
(0,1) (1,1) (2,1)             (0,1)    ↘    (1,0)
(0,2) (1,2) (2,2)       (0,2)    ↘     ↘    (2,0)
                              (1,2)    ↘   (2,1)
Position stored: (0,0)               (2,2)
Anchor placed at: back corner ★
```

## Layer System

### Render Layers

Tiles are assigned to layers that control draw order:

```go
layerOrder := []string{
    buildings.KindSeaID,              // 0 - Deep sea background
    buildings.KindGroundID + "_OVERLAY", // 1 - Ground under slopes/cliffs
    buildings.KindGroundID,           // 2 - Base terrain
    buildings.KindRoadsID,            // 3 - Roads
    buildings.KindForrestID,          // 4 - Trees (same priority as buildings)
    buildings.KindBuildingsID,        // 4 - Buildings (same priority as forest)
}

layerPriority := map[string]int{
    buildings.KindSeaID:                  0,
    buildings.KindGroundID + "_OVERLAY": 1,
    buildings.KindGroundID:              2,
    buildings.KindRoadsID:               3,
    buildings.KindForrestID:             4,  // Same as buildings
    buildings.KindBuildingsID:           4,  // Same as forest
}
```

### Why Forest and Buildings Share Priority

Trees and buildings can overlap (e.g., tree behind house). They must be depth-sorted together to render correctly:

```
Bad (separate layers):        Good (interleaved):
Draw all trees, then          Sort trees and buildings
all buildings                 together by depth, draw
                              back-to-front
```

## Depth Sorting

Within each layer priority, tiles are sorted by isometric depth:

```go
sort.Slice(renderableTiles, func(i, j int) bool {
    a, b := renderableTiles[i], renderableTiles[j]
    
    // First: sort by layer priority
    if a.Layer != b.Layer {
        return a.Layer < b.Layer
    }
    
    // Within same layer: sort by Y position (depth)
    if a.topY != b.topY {
        return a.topY < b.topY
    }
    
    // Tie-breaker: X position
    return a.topX < b.topX
})
```

**Depth principle:** Higher Y values are "closer" to camera, drawn later (on top):

```
   Y=0 ─────────────── (drawn first)
   Y=1 ───────────────
   Y=2 ─────────────── 
   Y=3 ─────────────── (drawn last, on top)
    ↓
  Viewer
```

## Occlusion Handling

Some tiles are hidden when occluded by buildings or roads:

### Building Footprint Occlusion

```go
// Track positions occupied by buildings
occupied := map[[2]int]struct{}{}
for _, tileEntry := range island.Tiles[buildings.KindBuildingsID] {
    pos := components.PositionType.Get(tileEntry)
    tile := components.TileType.Get(tileEntry)
    if tile.Occupation {
        occupied[[2]int{int(pos.X), int(pos.Y)}] = struct{}{}
    }
}

// Skip forest tiles under buildings
if layerID == buildings.KindForrestID {
    if _, onBuilding := occupied[posKey]; onBuilding {
        continue  // Don't draw this tree
    }
}
```

### Road Under Building

```go
// Roads are skipped if under a building AND base ground exists
if layerID == buildings.KindRoadsID {
    if _, ok := occupied[posKey]; ok {
        if _, hasBase := base[posKey]; hasBase {
            continue  // Skip road, ground shows through
        }
    }
}
```

## Sea Background

The deep sea extends infinitely beyond island bounds:

```go
func drawSeaBackground(screen, camera, island, seaImg, ...) {
    // Find sea tile (ID 1201)
    for _, tileEntry := range island.Tiles[buildings.KindSeaID] {
        b := components.BuildingType.Get(tileEntry)
        if b != nil && b.BuildingID == 1201 {
            seaImg = tile.Image
            break
        }
    }
    
    // Calculate visible grid range from camera viewport
    // Screen corners → isometric grid coords → tile range
    
    for y := iy0; y <= iy1; y++ {
        for x := ix0; x <= ix1; x++ {
            originX := (float64(x-y)) * (tileWidth/2) + baseX
            originY := (float64(x+y)) * (tileHeight/2) + baseY
            
            op := &ebiten.DrawImageOptions{}
            op.GeoM.Translate(originX-pivotX-camera.X, originY-pivotY-camera.Y)
            screen.DrawImage(seaImg, op)
        }
    }
}
```

## Camera System

The camera tracks viewport position for scrolling:

```go
type Camera struct {
    X float64  // World X offset
    Y float64  // World Y offset
}

// Applied during drawing
op.GeoM.Translate(tile.isoX - camera.X, tile.isoY - camera.Y)
```

## Rotation Support

When the world is rotated, tile positions are transformed:

```go
// Rotate local island position
lx := int(pos.X - island.X)
ly := int(pos.Y - island.Y)
rxl, ryl := rotation.RotatePosition(lx, ly, island.Width, island.Height, currentRotation)
rotatedX, rotatedY := rxl + int(island.X), ryl + int(island.Y)

// Then compute screen position from rotated coords
originX := ((float64(rotatedX)-island.X)-(float64(rotatedY)-island.Y)) * (tileWidth/2) + ...
```

## Sprite Rotation (Rotate=0 Buildings)

Some tiles need in-place sprite rotation when their COD Rotate field is 0:

```go
if tile.SpriteRotation > 0 {
    w, h := float64(tile.Image.Bounds().Dx()), float64(tile.Image.Bounds().Dy())
    op.GeoM.Translate(-w/2, -h/2)  // Move to center
    op.GeoM.Rotate(float64(tile.SpriteRotation) * math.Pi / 2)  // Rotate
    op.GeoM.Translate(w/2, h/2)    // Move back
}
```

## Height Offset

Tiles can have a vertical offset for elevation:

```go
originY -= pos.Offset  // Raise tile visually
```

## Debug Overlay

The overlay mode shows tile origins and layer info:

```go
if overlay {
    x := int(tile.originX - camera.X)
    y := int(tile.originY - camera.Y)
    text.Draw(screen, "+", basicfont.Face7x13, x-3, y+5, white)
    text.Draw(screen, fmt.Sprintf("%d", tile.Layer), basicfont.Face7x13, x+6, y+5, yellow)
}
```

## RenderableTile Structure

```go
type RenderableTile struct {
    isoX, isoY     float64       // Final draw position (screen coords)
    originX        float64       // Tile origin X (top of diamond)
    originY        float64       // Tile origin Y (top of diamond)
    bottomY        float64       // Bottom edge for depth (unused)
    topX, topY     int           // Grid position for sorting
    Image          *ebiten.Image // Sprite to draw
    Layer          int           // Layer priority (0-4)
    pivotX         float64       // Pivot X offset
    pivotY         float64       // Pivot Y offset
    SpriteRotation int           // 0-3 for 90° rotations
}
```

## Related Files

- `pkg/ecs/systems/renderer.go` - Main render system
- `pkg/ecs/components/camera.go` - Camera component
- `pkg/ecs/components/tile.go` - Tile component with Image, Pivot
- `pkg/world/rotation/rotation.go` - Rotation transformation
- `pkg/world/zoom/zoom.go` - Zoom level and tile sizes
