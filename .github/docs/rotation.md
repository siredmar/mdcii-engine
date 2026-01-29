# Rotation System

The rotation system handles two distinct types of rotation:
1. **World rotation** - Rotating the entire view (Q/E keys)
2. **Building rotation** - Per-building orientation (0-3, stored in savegame)

## Rotation Type

```go
// pkg/world/rotation/rotation.go
type Rotation int

const (
    DEG0   Rotation = iota  // 0 = No rotation
    DEG90                   // 1 = 90° clockwise
    DEG180                  // 2 = 180°
    DEG270                  // 3 = 270° clockwise (= 90° counter-clockwise)
)
```

## World Rotation

The entire island view can be rotated in 90° increments. This affects:
- How grid coordinates map to screen positions
- Which building rotation variant to display

### Controls

```go
// Q key: rotate clockwise
ctrl.Rotation.Increment()

// E key: rotate counter-clockwise
ctrl.Rotation.Decrement()
```

### Grid Transformation

When world rotation is applied, tile positions are transformed before rendering:

```go
// pkg/ecs/systems/renderer.go
// Rotate local island position
lx := int(pos.X - island.X)  // Local X within island
ly := int(pos.Y - island.Y)  // Local Y within island

// Apply rotation transform
rxl, ryl := rotation.RotatePosition(lx, ly, island.Width, island.Height, currentRotation)

// Convert back to world coords
rotatedX, rotatedY := rxl + int(island.X), ryl + int(island.Y)

// Then calculate screen position from rotated coords
originX := ((float64(rotatedX)-island.X)-(float64(rotatedY)-island.Y)) * (tileWidth/2) + ...
originY := ((float64(rotatedX)-island.X)+(float64(rotatedY)-island.Y)) * (tileHeight/2) + ...
```

### RotatePosition Function

Transforms a position within a rectangular grid:

```go
func RotatePosition(x, y, width, height int, rot Rotation) (int, int) {
    switch rot {
    case DEG0:
        return x, y
    case DEG90:
        // Clockwise 90°: (x, y) → (height - 1 - y, x)
        return height - 1 - y, x
    case DEG180:
        // 180°: (x, y) → (width - 1 - x, height - 1 - y)
        return width - 1 - x, height - 1 - y
    case DEG270:
        // 270° clockwise: (x, y) → (y, width - 1 - x)
        return y, width - 1 - x
    default:
        return x, y
    }
}
```

### Visual Example

```
Original (DEG0):          Rotated (DEG90):
    N                         W
  W + E                     S + N
    S                         E

Grid (0,0) at:            Grid (0,0) now at:
  upper-left                lower-left
```

## Building Rotation

Each building/tile has an orientation stored in the savegame (INSELHAUS field, bits 0-1).

### Combining Rotations

When rendering, building rotation and world rotation combine:

```go
// pkg/ecs/systems/animation.go
// Combine building and world rotation
effectiveRotation := building.Rotation.Add(globalRotation)

// Fetch correct sprite variant
frames := ani.GetAnimation(building.BuildingID, effectiveRotation).Frames
```

### Rotation Arithmetic

```go
// Add rotations (wraps at 4)
func (r Rotation) Add(other Rotation) Rotation {
    result := (int(r) + int(other)) % 4
    return IntToRotation(result)
}

// Subtract rotations (wraps at 4)
func (r Rotation) Subtract(other Rotation) Rotation {
    result := (int(r) - int(other) + 4) % 4
    return IntToRotation(result)
}

// Examples:
// DEG0 + DEG90 = DEG90
// DEG270 + DEG90 = DEG0  (wraps)
// DEG0 - DEG90 = DEG270  (wraps backwards)
```

## Sprite Rotation (Rotate=0 Buildings)

Some buildings have `Rotate=0` in COD, meaning they have only one set of sprites. When these buildings have a non-zero orientation, the sprite must be rotated at runtime:

```go
// pkg/ecs/components/island.go
if tileB.Rotate == 0 && currentTile.Orientation > 0 && !tileB.Kind.IsWater() && !tileB.Kind.IsForrest() {
    spriteRot = currentTile.Orientation
}

TileType.Set(tileEntry, &Tile{
    SpriteRotation: spriteRot,  // 0-3
    // ...
})
```

At render time:

```go
// pkg/ecs/systems/renderer.go
if tile.SpriteRotation > 0 {
    w, h := float64(tile.Image.Bounds().Dx()), float64(tile.Image.Bounds().Dy())
    // Rotate around image center
    op.GeoM.Translate(-w/2, -h/2)
    op.GeoM.Rotate(float64(tile.SpriteRotation) * math.Pi / 2)
    op.GeoM.Translate(w/2, h/2)
}
```

### Exclusions

Certain tile types are excluded from sprite rotation because their orientation field doesn't indicate visual rotation:
- **Sea tiles**: Orientation is wave animation phase, not direction
- **Forest tiles**: Trees are symmetrical, orientation is random variant

## Building Rotation Offsets

Multi-tile buildings need offset tables to correctly place each tile after rotation:

```go
// pkg/building/building.go
var RotationOffsets = map[BuildingSizeIdentifier]map[int][][2]int{
    BuildingSizeIdentifier_2x2: {
        0: {{0, 0}, {1, 0}, {0, 1}, {1, 1}},  // Rotation 0
        1: {{0, 1}, {0, 0}, {1, 1}, {1, 0}},  // Rotation 1
        2: {{1, 1}, {0, 1}, {1, 0}, {0, 0}},  // Rotation 2
        3: {{1, 0}, {1, 1}, {0, 0}, {0, 1}},  // Rotation 3
    },
    // ... other sizes
}
```

These offsets define which grid cells each sprite index occupies at each rotation.

## Coordinate Conversion

### Cartesian to Isometric

```go
func CartesianToIso(x, y float64, tileSize int) (float64, float64) {
    rx := (x - y) * float64(tileSize/2)
    ry := (x + y) * float64(tileSize/4)
    return rx, ry
}
```

### Isometric to Cartesian

```go
func IsoToCartesian(x, y float64, tileSize int) (float64, float64) {
    rx := (x/float64(tileSize/2) + y/float64(tileSize/4)) / 2
    ry := (y/float64(tileSize/4) - (x / float64(tileSize/2))) / 2
    return rx, ry
}
```

## Summary Table

| Rotation | Degrees | Transform (x,y → newX,newY) |
|----------|---------|---------------------------|
| DEG0 | 0° | (x, y) |
| DEG90 | 90° CW | (height - 1 - y, x) |
| DEG180 | 180° | (width - 1 - x, height - 1 - y) |
| DEG270 | 270° CW | (y, width - 1 - x) |

## Related Files

- `pkg/world/rotation/rotation.go` - Rotation type and transforms
- `pkg/ecs/systems/renderer.go` - World rotation application
- `pkg/ecs/systems/animation.go` - Building + world rotation combination
- `pkg/building/building.go` - RotationOffsets for multi-tile buildings
- `pkg/texture/atlas/atlas.go` - Pre-rendering all 4 rotation variants
