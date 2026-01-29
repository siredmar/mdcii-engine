# Copilot Instructions for mdcii-engine

This is a Go reimplementation of the Anno 1602/1602 AD game engine. It loads original game assets (sprites, building definitions, savegames) and renders islands with animated buildings using an ECS architecture and the Ebiten game library.

---

## TL;DR - Essential Facts

**What it does:** Loads Anno 1602 game files → parses into ECS world → renders isometric view

**Key formulas:**
```go
screenX = (gridX - gridY) * 32    // Isometric X
screenY = (gridX + gridY) * 16    // Isometric Y
drawX = screenX - pivotX          // Sprite positioning
```

**Critical conventions:**
- Origin = **top point** of isometric diamond
- Buildings anchor at **back corner** (grid 0,0)
- Sentinel `0xFFFF` = empty/use base layer
- Layer order: Sea(0) → Ground(2) → Roads(3) → Forest/Buildings(4)

**Build & Run:**
```bash
make                                           # Build
go run cmd/island_ecs/main.go -p /path/to/anno # Run viewer
```

---

## Detailed Documentation

For in-depth information, see `.github/docs/`:

| Document | When to Read |
|----------|--------------|
| [bsh-format.md](docs/bsh-format.md) | Working with sprites, RLE decoding |
| [cod-format.md](docs/cod-format.md) | Building definitions, Kind types |
| [chunk-format.md](docs/chunk-format.md) | Savegame parsing, INSEL5/INSELHAUS |
| [islands.md](docs/islands.md) | Island layers, Finalize() merging |
| [rendering.md](docs/rendering.md) | Isometric math, depth sorting |
| [texture-atlas.md](docs/texture-atlas.md) | Atlas generation, pivot calculation |
| [ecs-architecture.md](docs/ecs-architecture.md) | Components, systems, queries |
| [rotation.md](docs/rotation.md) | World/building rotation |

---

## Quick Reference

### Build Commands
```bash
make                    # Build (runs tests first)
go test ./...           # Run all tests
go test ./pkg/chunks -run TestIsland5  # Single test
```

### Run Viewer
```bash
go run cmd/island_ecs/main.go -p /path/to/anno1602      # Basic
go run cmd/island_ecs/main.go -p /path/to/anno1602 -r 1 # With rotation
```

### Viewer Controls
| Key | Action |
|-----|--------|
| WASD/Arrows | Pan camera |
| Q/E | Rotate world |
| G | Toggle debug grid |
| O | Toggle origin markers |
| Escape | Exit |

### File Locations by Task
| Task | Files | Doc |
|------|-------|-----|
| Sprites | `pkg/bsh/`, `pkg/texture/atlas/` | bsh-format.md, texture-atlas.md |
| Buildings | `pkg/cod/`, `pkg/cod/buildings/` | cod-format.md |
| Savegames | `pkg/chunks/`, `pkg/gam/` | chunk-format.md, islands.md |
| Rendering | `pkg/ecs/systems/renderer.go` | rendering.md |
| ECS | `pkg/ecs/components/`, `pkg/ecs/systems/` | ecs-architecture.md |
| Rotation | `pkg/world/rotation/` | rotation.md |

---

## Architecture Overview

### Data Flow Pipeline

```
Original Game Files → Parsing → ECS World → Rendering
       ↓                ↓           ↓           ↓
  BSH sprites      Buildings    Entities    Ebiten
  COD definitions  Islands      Components  Screen
  GAM/SCP saves    Chunks       Systems
```

### ECS (Entity-Component-System) with Donburi

The engine uses **github.com/yohamta/donburi** for game state:

**Components** (`pkg/ecs/components/`):
- `Building`: BuildingID, Rotation, Size - links entity to COD building definition
- `Tile`: Current sprite image, PivotX/Y for positioning, SpriteRotation for runtime rotation
- `Position`: X, Y grid coordinates, Offset for elevation
- `Animation`: CurrentFrame, Count, Duration, Loop, Running state
- `Island`: Width, Height, world position, Climate, map of layer→tile entries
- `Camera`: X, Y scroll offset, Zoom level
- `Control`: Global rotation state, grid visibility, input state

**Systems** (`pkg/ecs/systems/`):
- `RenderSystem`: Queries islands, sorts tiles by layer+depth, draws to screen
- `AnimationSystem`: Updates animation frames based on deltaTime, sets tile images
- `InputSystem`: Handles WASD/arrow camera movement, Q/E rotation, mouse drag

**World** (`pkg/ecs/world/`):
- Thin wrapper around `donburi.World`
- Created once at startup, entities added for islands, camera, control

### Isometric Coordinate System

Tiles use a **64×32 diamond** isometric projection:

```go
// Grid (x, y) to screen coordinates
originX = (x - y) * (tileWidth / 2)   // tileWidth = 64
originY = (x + y) * (tileHeight / 2)  // tileHeight = 32

// Screen to grid (inverse)
x = (screenX/32 + screenY/16) / 2
y = (screenY/16 - screenX/32) / 2
```

**Key concepts:**
- Origin is the **top point** of the isometric diamond
- Buildings anchor at the **back corner** (grid 0,0 of the building footprint)
- Pivot points (PivotX, PivotY) offset sprites so the anchor aligns with screen origin
- Depth sorting: lower layer priority first, then by (topY, topX) for same layer

### Anno 1602 File Formats

#### BSH Files (`pkg/bsh/`)
Compressed sprite archives containing indexed-color images.

**Structure:**
- 20-byte header: magic ID, payload length, image count
- Image offset table (4 bytes per image)
- Per-image: 16-byte header (width, height, type, length) + RLE pixel data

**RLE Decoding** (in `png.go`):
- `0xFF`: End of image
- `0xFE`: End of row, move to next line
- Other: Skip N pixels, then read next byte for pixel count, then N palette indices

**Usage:**
```go
bshPng, _ := bsh.NewPng(bsh.WithFile("stadtfld.bsh"), bsh.WithConvertAll())
img := bshPng.Images["123"]  // Access by string index
```

#### COD Files (`pkg/cod/`)
XOR-encoded text files with building definitions. Main file: `haeuser.cod`

**Parsing** (`parser.go`):
- Each byte XOR'd with -1 to decode
- Line-based format with object hierarchy
- Supports: constants, variables, arrays, includes, object definitions

**Key building fields** (`pkg/cod/buildings/`):
```go
type Building struct {
    Id              int           // Unique building ID
    Gfx             int           // Base sprite index in BSH
    Size            BuildingSize  // Footprint {W, H} in tiles
    Rotate          int           // Sprites per rotation (0 = symmetric)
    AnimationAmount int           // Number of animation frames
    AnimationAdd    int           // Sprite index offset per frame
    AnimationTime   int           // Animation speed
    Kind            Kind          // Category (Building, Ground, Sea, etc.)
    PositionOffset  int           // Vertical elevation offset
}
```

**Tile Kinds** (`types.go`):
- `KindSea`: Water tiles (Sea, Surf, Estuary)
- `KindGround`: Terrain (Ground, Beach, Slope, Rock, River)
- `KindForrest`: Trees
- `KindRoads`: Streets, Bridges, Plazas
- `KindBuildings`: Structures (Building, Wall, Harbor, Tower, etc.)

#### GAM/SCP Files (`pkg/gam/`, `pkg/chunks/`)
Binary savegames (.gam) and scenarios (.scp) using chunk format.

**Chunk Structure** (`chunk.go`):
```go
type Chunk struct {
    Id     string  // 16-byte null-terminated ID like "INSEL5"
    Length int     // Payload length
    Data   []byte  // Raw chunk data
}
```

**Island Chunks:**
- `INSEL5` (116 bytes): Island metadata - position, dimensions, climate, fertility, ore deposits
- `INSEL3`, `INSEL4`: Older format variants, converted to INSEL5
- `INSELHAUS`: Tile grid - 8 bytes per field

**Field Structure** (`islandhouse.go`):
```go
type Field struct {
    Id             int   // Building/tile ID from COD
    Posx, Posy     int   // Position within island
    Orientation    int   // 0-3 rotation (2 bits)
    AnimationCount int   // Current animation frame (4 bits)
    IslandNumber   int   // Island index (8 bits)
    CityNumber     int   // City index (3 bits)
    PlayerNumber   int   // Owner player (4 bits)
}
```

**Layer Merging** (`Island5.Finalize()`):
1. Load base terrain from `.scp` template file
2. Load overlay (player modifications) from savegame
3. Merge: overlay takes priority unless `Id == 0xFFFF` (sentinel for "use base")

### Texture Atlas System (`pkg/texture/atlas/`)

Pre-renders all buildings with rotations and animation frames into sprite sheets.

**Atlas Generation:**
1. For each building in COD definitions:
2. For each rotation (0, 90, 180, 270):
3. For each animation frame:
4. Composite tile sprites onto canvas using isometric offsets
5. Calculate union bounds across all frames (stable cropping)
6. Compute pivot = anchor point - crop bounds origin
7. Pack into 2048×2048 PNG sheets

**Pivot Calculation:**
```go
// Anchor at back corner (local 0,0)
anchor := image.Point{X: buildingX, Y: buildingY}
// Pivot = offset from cropped image origin to anchor
pivotX := anchor.X - cropBounds.Min.X
pivotY := anchor.Y - cropBounds.Min.Y
```

**Caching:**
- Saved to `/tmp/atlas/texture-atlas.json` + PNG sheets
- Version field (currently 3) triggers rebuild on format changes
- Delete `/tmp/atlas/` to force regeneration

### Animation System (`pkg/texture/animations/`)

Wraps atlas data for runtime use:
```go
type Animation struct {
    Frames        []*ebiten.Image  // Pre-converted Ebiten images
    FrameMeta     []atlas.Metadata // Pivot info per frame
    Steps         int              // Frame count
    FrameDuration int              // Milliseconds per frame
}
```

**AnimationSystem** updates tiles each frame:
1. Query entities with Building + Animation + Tile components
2. Calculate frame index from elapsed time
3. Set `tile.Image` to current frame
4. Update `tile.PivotX/Y` from frame metadata

### Rotation System (`pkg/world/rotation/`)

Four discrete rotations representing 90° increments:
```go
type Rotation int
const (DEG0, DEG90, DEG180, DEG270 Rotation = 0, 1, 2, 3)
```

**Coordinate Transforms:**
```go
// Rotate local position (x, y) within bounds (width, height)
func RotatePosition(x, y, width, height int, rot Rotation) (int, int) {
    switch rot {
    case DEG0:   return x, y
    case DEG90:  return height - 1 - y, x
    case DEG180: return width - 1 - x, height - 1 - y
    case DEG270: return y, width - 1 - x
    }
}
```

**Global vs Local Rotation:**
- Buildings have inherent rotation from savegame (`building.Rotation`)
- Player can rotate entire view (`control.Rotation`)
- Final rotation = `building.Rotation.Add(globalRotation)`

### File Resolution (`pkg/files/`)

Singleton for locating game assets:
```go
files.CreateInstance("/path/to/anno1602")  // Initialize once
path, _ := files.Instance().FindPathForFile("haeuser.cod")  // Case-insensitive search
```

Builds a tree of all files at startup, searches by substring match.

## Rendering Pipeline

### Layer Order and Depth Sorting

```go
layerOrder := []string{
    KindSeaID,              // Priority 0
    KindGroundID+"_OVERLAY", // Priority 1 (underlays for slopes)
    KindGroundID,           // Priority 2
    KindRoadsID,            // Priority 3
    KindForrestID,          // Priority 4 (same as buildings)
    KindBuildingsID,        // Priority 4 (interleaved with forest)
}
```

**Within same layer:** Sort by `(topY, topX)` for correct overlap.

### Occlusion Handling

- Buildings mark their footprint cells as "occupied"
- Forest tiles skip rendering on occupied cells
- Roads skip only if a building covers them AND ground exists underneath
- Slope/cliff tiles get a ground underlay to fill transparent areas

### Sea Background

Deep sea tile (ID 1201) tiles infinitely behind islands using camera bounds calculation.

## island_ecs Program

The main viewer demonstrating the full rendering pipeline.

### Startup Flow
1. Initialize `files.Instance()` with game root path
2. Parse `haeuser.cod` → building definitions
3. Load `stadtfld.bsh` → sprite images
4. Generate or load cached texture atlas from `/tmp/atlas/`
5. Parse savegame (`SAVEGAME/lastgame.gam`) using chunk parser
6. Create ECS world with island entities from parsed chunks
7. Run Ebiten game loop with render/animation/input systems

### Island Layer Merging
Islands have two layers merged in `Island5.Finalize()`:
- **Final[0]**: Base terrain from `.scp` island template files
- **Final[1]**: Overlay with player modifications (buildings, roads)

Sentinel value `0xFFFF` in overlay means "use base layer".

---

## Key Conventions

| Convention | Details |
|------------|---------|
| Binary parsing | `structex` for bitfields, `binstruct` for chunks. Field order must match Anno's layout exactly. |
| Error handling | `panic()` on init failures, log+skip on runtime errors |
| Atlas cache | `/tmp/atlas/` - delete to regenerate. Version 3 triggers rebuild. |
| Sprite index | `baseGfx + (rotation * rotateStride) + (animFrame * animAdd)` |

---

## Common Tasks

| Task | Steps |
|------|-------|
| Debug rendering | Press `G` (grid) + `O` (origins) in viewer |
| Regenerate atlas | `rm -rf /tmp/atlas && go run cmd/island_ecs/main.go -p /path` |
| Add building kind | Edit `KindMap` in `pkg/cod/buildings/types.go` |
| Test savegame | `go run cmd/island_ecs/main.go -p /path` (loads lastgame.gam) |
