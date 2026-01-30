# Texture Atlas System

The texture atlas pre-renders all building sprites into packed PNG sheets with metadata for efficient runtime loading.

## Overview

The atlas:
1. Iterates all buildings from COD definitions
2. Renders each building for all 4 rotations and all animation frames
3. Computes stable crop bounds across animation frames
4. Calculates pivot points relative to anchor (back corner)
5. Packs sprites into 2048×2048 PNG sheets
6. Exports JSON metadata for runtime loading

## Atlas Structure

### Main Type

```go
// pkg/texture/atlas/atlas.go
type TextureAtlas struct {
    Images     []*image.RGBA              // PNG sheet images
    AtlasMeta  AtlasMeta                  // Global metadata
    ImagesMeta map[int]*ImageSetRotation  // Per-building metadata
    PNGs       *bsh.BshPng                // Source sprites from BSH
    BuildingsCOD buildingsCOD.Buildings   // Building definitions
    outputDir  string                     // Export directory
}

type AtlasMeta struct {
    Width   int    `json:"width"`   // Sheet width (2048)
    Height  int    `json:"height"`  // Sheet height (2048)
    Name    string `json:"name"`    // Atlas name
    Version int    `json:"version"` // Cache version (currently 3)
}
```

### Per-Building Metadata

```go
type ImageSetRotation struct {
    Animations map[rotation.Rotation]*Animation  // 4 rotations (0-3)
}

type Animation struct {
    Images []Image       // Frames for this rotation
    Steps  int           // Number of animation steps
    Time   time.Duration // Frame duration
}

type Image struct {
    Sprite   image.Image  // The actual sprite image
    Metadata Metadata     // Position and pivot data
}

type Metadata struct {
    BuildingID     int  // COD building ID
    PNGIndex       int  // Which PNG sheet contains this sprite
    X, Y           int  // Position within sheet
    Width, Height  int  // Sprite dimensions
    PivotX, PivotY int  // Anchor offset for positioning
    Rotation       int  // Rotation index (0-3)
    AnimationIndex int  // Frame index
}
```

## Building Rendering

### Canvas Setup

Each building is rendered onto a large canvas (1000×1000) then cropped:

```go
func (a *TextureAtlas) renderBuildingCanvas(b *building.Building, tileSize TileSize) (*image.RGBA, image.Point) {
    outputImage := image.NewRGBA(image.Rect(0, 0, 1000, 1000))
    
    // Anchor at back corner (0,0 in local building coords)
    // This is where Anno 1602 stores the building position
    anchorX := b.X  // e.g., 100
    anchorY := b.Y  // e.g., 100
    anchor := image.Point{X: anchorX, Y: anchorY}
    
    // Get tile offsets for this rotation (computed dynamically)
    offsets := building.GenerateTileOffsets(b.Size.Width(), b.Size.Height(), b.Rotation)
    
    for i, offset := range offsets {
        // Calculate isometric screen position for each tile
        screenX := b.X + (offset[0]-offset[1]) * (tileWidth/2)
        screenY := b.Y + (offset[0]+offset[1]) * (tileHeight/2)
        
        // Get sprite index from COD
        spriteIdx := baseIndex + (rotation * rotateStride) + tileIndex
        tileImg := a.PNGs.Images[spriteIdx]
        
        // Tall sprites need vertical offset
        baseOffsetY := max(0, tileImg.Bounds().Dy() - tileHeight)
        
        // Draw tile sprite
        draw.Draw(outputImage, 
            image.Rect(screenX, screenY-baseOffsetY, screenX+w, screenY-baseOffsetY+h),
            tileImg, image.Point{}, draw.Over)
    }
    
    return outputImage, anchor
}
```

### Multi-Tile Buildings

Building footprints are defined by size (W×H). The `GenerateTileOffsets` function computes grid offsets for each tile by applying rotation transforms:

```go
// pkg/building/rotation.go
func GenerateTileOffsets(width, height, rotation int) [][2]int {
    offsets := make([][2]int, 0, width*height)
    for y := 0; y < height; y++ {
        for x := 0; x < width; x++ {
            rx, ry := rotatePosition(x, y, width, height, rotation)
            offsets = append(offsets, [2]int{rx, ry})
        }
    }
    return offsets
}

// Example: 2x2 building at rotation 0 → {{0,0}, {1,0}, {0,1}, {1,1}}
// Example: 2x2 building at rotation 1 → {{1,0}, {1,1}, {0,0}, {0,1}}
```

## Pivot Calculation

The pivot point enables correct sprite positioning during rendering.

### Anchor Convention

The **anchor** is placed at the **back corner** of the building:
- Local grid position (0, 0)
- Where Anno stores the building's X,Y coordinate
- Top point of the back tile's isometric diamond

```
2x2 Building:
    Grid:                Isometric:
    (0,0) (1,0)               ★ (0,0) = anchor
    (0,1) (1,1)        (0,1)    ↘    (1,0)
                            (1,1)
```

### Crop and Pivot

```go
// Union bounds across all animation frames
var unionBounds image.Rectangle
for each animation frame:
    canvas := renderBuildingCanvas(...)
    bounds := findNonAlphaBounds(canvas)
    unionBounds = unionBounds.Union(bounds)

// Pivot = offset from cropped image corner to anchor
anchor := image.Point{X: b.X, Y: b.Y}  // Back corner
pivotX := anchor.X - unionBounds.Min.X
pivotY := anchor.Y - unionBounds.Min.Y

// Crop to union bounds
croppedImage := cropImage(canvas, unionBounds)
```

### Visual Explanation

```
Full canvas (1000×1000):
┌────────────────────────────────────────────┐
│                                            │
│     ★ anchor (100, 100)                   │
│     ┌────────────────────┐                │
│     │    Sprite pixels   │ unionBounds    │
│     │  █████████████████ │                │
│     │ ████████████████████│               │
│     │  █████████████████ │                │
│     └────────────────────┘                │
│                                            │
└────────────────────────────────────────────┘

Cropped:
┌────────────────────┐
│ ★ ← pivot offset   │  pivotX = 100 - unionBounds.Min.X
│    Sprite pixels   │  pivotY = 100 - unionBounds.Min.Y
│  █████████████████ │
│ ████████████████████│
│  █████████████████ │
└────────────────────┘
```

## Stable Animation Bounds

Animation frames can have slightly different content. To prevent "jitter", all frames use the same crop bounds:

```go
// First pass: compute union of all frame bounds
for animationStep := 0; animationStep < animations; animationStep++ {
    canvas := renderBuildingCanvas(b, tileSize)
    bounds := findNonAlphaBounds(canvas)
    frames = append(frames, renderedFrame{canvas: canvas, contentBounds: bounds})
    unionBounds = unionBounds.Union(bounds)
}

// Second pass: crop all frames to union bounds
for animationStep := 0; animationStep < animations; animationStep++ {
    cropped := cropImage(frames[animationStep].canvas, unionBounds)
    // Same pivotX, pivotY for all frames
}
```

## Sprite Packing

Sprites are packed into 2048×2048 sheets using simple row-based packing:

```go
const sheetWidth, sheetHeight = 2048, 2048

for _, entry := range allImages {
    w, h := img.Bounds().Dx(), img.Bounds().Dy()
    
    // Check if fits in current row
    if x + w > sheetWidth {
        x = 0
        y += maxRowHeight
        maxRowHeight = 0
    }
    
    // Check if fits in current sheet
    if y + h > sheetHeight {
        sheetIndex++
        sheets = append(sheets, newSheet())
        y = 0
        x = 0
    }
    
    // Draw to sheet
    draw.Draw(sheets[sheetIndex], image.Rect(x, y, x+w, y+h), img, ...)
    
    // Record position in metadata
    entry.meta.PNGIndex = sheetIndex
    entry.meta.X = x
    entry.meta.Y = y
    
    x += w
    if h > maxRowHeight { maxRowHeight = h }
}
```

## Export Format

### JSON Metadata

```json
{
  "atlasMeta": {
    "width": 2048,
    "height": 2048,
    "name": "texture-atlas",
    "version": 3
  },
  "imageMeta": {
    "101": {  // Building ID
      "Animations": {
        "0": {  // Rotation
          "Images": [
            {
              "metadata": {
                "buildingID": 101,
                "pngIndex": 0,
                "x": 0,
                "y": 0,
                "width": 64,
                "height": 48,
                "pivotX": 32,
                "pivotY": 16,
                "rotation": 0,
                "animationIndex": 0
              }
            }
          ],
          "Steps": 1,
          "Time": 0
        },
        "1": { ... },  // Rotation 1
        "2": { ... },  // Rotation 2
        "3": { ... }   // Rotation 3
      }
    },
    "102": { ... }
  }
}
```

### PNG Sheets

Output files:
- `texture-atlas.json` - Metadata
- `texture-atlas-0000.png` - First sprite sheet
- `texture-atlas-0001.png` - Second sheet (if needed)
- ...

## Loading from Cache

```go
func LoadAtlasFromJSON(jsonPath string) (*TextureAtlas, error) {
    // Load metadata
    var exportMeta ExportMeta
    json.NewDecoder(jsonFile).Decode(&exportMeta)
    
    atlas := &TextureAtlas{
        AtlasMeta:  exportMeta.AtlasMeta,
        ImagesMeta: exportMeta.ImagesMeta,
    }
    
    // Load PNG sheets
    for i := 0; ; i++ {
        pngPath := fmt.Sprintf("%s-%04d.png", basePath, i)
        if !exists(pngPath) { break }
        img := loadImage(pngPath)
        atlas.Images = append(atlas.Images, img)
    }
    
    // Reconstruct Sprite references from sheet regions
    for _, set := range atlas.ImagesMeta {
        for _, anim := range set.Animations {
            for i := range anim.Images {
                meta := &anim.Images[i].Metadata
                sheet := atlas.Images[meta.PNGIndex]
                rect := image.Rect(meta.X, meta.Y, meta.X+meta.Width, meta.Y+meta.Height)
                anim.Images[i].Sprite = sheet.SubImage(rect)
            }
        }
    }
    
    return atlas, nil
}
```

## Caching

The atlas is cached at `/tmp/atlas/`:

```go
// pkg/texture/texture.go
const atlasPath = "/tmp/atlas/texture-atlas.json"
const currentVersion = 3

func loadOrCreateAtlas() *atlas.TextureAtlas {
    // Try to load cached version
    if exists(atlasPath) {
        cached, err := atlas.LoadAtlasFromJSON(atlasPath)
        if err == nil && cached.AtlasMeta.Version == currentVersion {
            return cached  // Use cached atlas
        }
    }
    
    // Build new atlas from scratch
    newAtlas := atlas.New(2048, 2048, buildingsDef, 
        atlas.WithImages(bshImages),
        atlas.WithOutputDir("/tmp/atlas"))
    
    newAtlas.Export()  // Save for future runs
    return newAtlas
}
```

### Version Invalidation

When the Version field changes (currently 3), cached atlases are rebuilt:

```go
if cached.AtlasMeta.Version != currentVersion {
    // Rebuild from scratch
}
```

This ensures format changes are properly applied.

## Related Files

- `pkg/texture/atlas/atlas.go` - Atlas generation and loading
- `pkg/texture/texture.go` - Texture manager with caching
- `pkg/building/building.go` - Building struct and size helpers
- `pkg/building/rotation.go` - GenerateTileOffsets function
- `pkg/bsh/bsh_png.go` - Source sprite loading
