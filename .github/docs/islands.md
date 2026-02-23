# Island Structure and Layer System

Islands in Anno 1602 are rectangular grids of tiles with a two-layer system that separates base terrain from player modifications.

## Island Data Model

### Island5 Structure

```go
// pkg/chunks/island5.go
type Island5 struct {
    island5Data          // Embedded metadata (116 bytes from chunk)
    Layers    layers     // Tile layers
    Buildings *buildings.Buildings  // Reference to building definitions
}

type layers struct {
    Top         *IslandHouse   // Final merged layer for rendering
    Bottom      *IslandHouse   // Base terrain (unused after merge)
    Final       []*IslandHouse // [0]=base, [1]=overlay before merge
    IslandHouse []*IslandHouse // Raw INSELHAUS chunks
}
```

### Key Metadata Fields

| Field | Description |
|-------|-------------|
| `IslandNumber` | Unique ID for this island in the game |
| `Width`, `Height` | Grid dimensions in tiles |
| `Posx`, `Posy` | World position (top-left corner) |
| `Climate` | 0=North (temperate), 1=South (tropical) |
| `Size` | Category: Little, Middle, Big, Huge |
| `ModifiedFlag` | Whether island has player changes |
| `FileNumber` | Template file number |
| `Fertility` | Bitfield of available crops |

## Two-Layer System

Anno uses two layers to efficiently store islands:

### Base Layer (Template)
- Contains original terrain from `.scp` template files
- Includes: ground, coast, slopes, native trees
- Shared across all saves using same island template
- Stored in game files under `NORDNAT/` or `SUEDNAT/`

### Overlay Layer (Modifications)
- Contains player changes: buildings, roads, cleared forest
- Stored in savegame
- Uses sentinel value `0xFFFF` for "use base layer"

## Layer Merging (`Island5.Finalize()`)

The `Finalize()` method merges layers into a single `Top` layer for rendering:

```go
func (i *Island5) Finalize() error {
    // Case 1: Unmodified island with 1 or fewer INSELHAUS chunks
    if i.ModifiedFlag == ModifiedFalse && len(i.Layers.IslandHouse) <= 1 {
        // Load base terrain from template .scp file
        islandFile := i.IslandFileName()  // e.g., "NORDNAT/LIT02.SCP"
        
        chunksFromFile, _ := NewChunksFromFile(path)
        foundChunk, _ := FindChunkById(chunksFromFile, "INSELHAUS")
        inselHouse, _ := NewIslandHouse(foundChunk, ...)
        
        i.Layers.IslandHouse = append(i.Layers.IslandHouse, inselHouse)
    }
    
    // Set up Final array: [0]=base, [1]=overlay
    if len(i.Layers.IslandHouse) == 2 {
        i.Layers.Final = []*IslandHouse{
            i.Layers.IslandHouse[0],  // Base
            i.Layers.IslandHouse[1],  // Overlay
        }
    } else if len(i.Layers.IslandHouse) == 1 {
        // Only base layer exists, create empty overlay
        empty := NewEmptyIslandHouse(IslandDimensions{i.Width, i.Height})
        i.Layers.Final = []*IslandHouse{
            i.Layers.IslandHouse[0],  // Base
            empty,                     // Empty overlay
        }
    }
    
    // Merge: overlay takes priority unless 0xFFFF
    i.Layers.Top = NewEmptyIslandHouse(...)
    for index := range i.Layers.Final[0].Fields {
        overlay := i.Layers.Final[1].Fields[index]
        base := i.Layers.Final[0].Fields[index]
        
        if overlay.Id != 0xFFFF {
            i.Layers.Top.Fields[index] = overlay
        } else {
            i.Layers.Top.Fields[index] = base
        }
    }
    
    return nil
}
```

### Merge Priority

```
For each cell (x, y):
    if overlay[x,y].Id != 0xFFFF:
        result[x,y] = overlay[x,y]
    else:
        result[x,y] = base[x,y]
```

## IslandHouse (Tile Grid)

The `IslandHouse` structure holds the actual tile grid:

```go
type IslandHouse struct {
    Size        IslandDimensions
    Fields      []Field              // Processed grid (width × height)
    RawFields   []Field              // Raw parsed fields from chunk
    Buildings   *buildings.Buildings
}
```

### Field Structure

Each tile in the grid:

```go
type Field struct {
    Id             int   // Building/tile ID from COD (0xFFFF = empty)
    Posx           int   // X position within island
    Posy           int   // Y position within island
    Orientation    int   // Rotation (0-3)
    AnimationCount int   // Current animation frame
    IslandNumber   int   // Island index
    CityNumber     int   // City index
    RandomNumber   int   // Random variant seed
    PlayerNumber   int   // Owner player (0-7)
    Kind           Kind  // Tile category (set during parsing)
}
```

### Grid Initialization

Empty grids are initialized with sentinel values:

```go
func (i *IslandHouse) finalize() {
    // Initialize all cells to empty
    i.Fields = make([]Field, i.Size.Height * i.Size.Width)
    for y := 0; y < i.Size.Height; y++ {
        for x := 0; x < i.Size.Width; x++ {
            i.Fields[y*i.Size.Width+x] = Field{
                Id:   0xFFFF,  // Empty sentinel
                Posx: x,
                Posy: y,
            }
        }
    }
    
    // Place parsed fields
    for _, tile := range i.RawFields {
        idx := tile.Posy*i.Size.Width + tile.Posx
        i.Fields[idx] = tile
    }
}
```

### Accessing Tiles

```go
// Get tile at position
tile := island.Layers.Top.Get(x, y)

// Implementation
func (i *IslandHouse) Get(x, y int) Field {
    return i.Fields[y*i.Size.Width+x]
}
```

## Island File Templates

Unmodified islands load base terrain from template files:

```go
func (i *Island5) IslandFileName() string {
    // Climate directory
    climate := IslandClimateMap[i.Climate]  // "NORDNAT" or "SUEDNAT"
    
    // Size prefix
    size := IslandSizeMap[i.Size]  // "LIT", "MIT", "GRO", "GRS"
    
    // File number with padding
    return fmt.Sprintf("%s/%s%02d.scp", climate, size, i.FileNumber)
    // e.g., "NORDNAT/LIT02.SCP", "SUEDNAT/GRO05.SCP"
}
```

### Climate Directories

| Climate | Directory | Description |
|---------|-----------|-------------|
| 0 (North) | `NORDNAT/` | Temperate islands |
| 1 (South) | `SUEDNAT/` | Tropical islands |

### Size Prefixes

| Size | Prefix | Approximate Dimensions |
|------|--------|----------------------|
| Little | `LIT` | ~20x20 tiles |
| Middle | `MIT` | ~30x30 tiles |
| Big | `GRO` | ~50x50 tiles |
| Huge | `GRS` | ~70x70 tiles |

## Modified vs Unmodified Islands

### Unmodified (`ModifiedFlag == 0`)

- Base terrain loaded from `.scp` template
- Savegame may contain overlay with modifications
- Both layers merged at runtime

### Modified (`ModifiedFlag == 1`)

- Full island data stored in savegame
- First INSELHAUS chunk is base
- Second INSELHAUS chunk (if present) is overlay

```go
if i.ModifiedFlag == ModifiedFalse && len(i.Layers.IslandHouse) <= 1 {
    // Load from template file
    islandFile := i.IslandFileName()
    // ...
} else {
    // Use chunks from savegame directly
    i.Layers.Final = append(i.Layers.Final, i.Layers.IslandHouse[0])
    if len(i.Layers.IslandHouse) == 2 {
        i.Layers.Final = append(i.Layers.Final, i.Layers.IslandHouse[1])
    }
}
```

## Fertility Flags

The `Fertility` field is a bitfield indicating which crops can grow:

```go
type Fertility int

const (
    Random       Fertility = iota  // Random selection
    Tobacco                        // Tobacco
    Spices                         // Spices
    Sugar                          // Sugar cane
    Cotton                         // Cotton
    Wine                           // Wine/grapes
    Cocoa                          // Cocoa
    // ...
)
```

## Ore Mountains

Each island can have up to 4 iron deposits and 4 volcano deposits:

```go
type OreMountainData struct {
    Pos       int `bitfield:"16"`  // Packed X,Y position
    Amount    int `bitfield:"16"`  // Ore amount remaining
    Kind      int `bitfield:"8"`   // Ore type
    Playernum int `bitfield:"8"`   // Claiming player
    Reserved  int `bitfield:"16,reserved"`
}

// In island5Data:
Eisenberg  [4]OreMountainData  // Iron deposits
Vulkanberg [4]OreMountainData  // Volcano (gold) deposits
```

## Creating Islands from Chunks (ECS)

The `CreateIslandFromChunk()` function converts parsed island data into ECS entities:

```go
// pkg/ecs/components/island.go
func CreateIslandFromChunk(world donburi.World, ani *animations.Animations, 
                           i *island5.Island5, worldX, worldY float64) *donburi.Entry {
    // Create island entity
    islandEntity := world.Create(IslandType)
    islandEntry := world.Entry(islandEntity)
    
    island := &Island{
        Width:   i.Width,
        Height:  i.Height,
        X:       worldX,
        Y:       worldY,
        Climate: Climate(i.Climate),
        Tiles:   map[string][]*donburi.Entry{},
    }
    
    // Initialize layer maps
    island.Tiles[buildings.KindBuildingsID] = []*donburi.Entry{}
    island.Tiles[buildings.KindSeaID] = []*donburi.Entry{}
    island.Tiles[buildings.KindGroundID] = []*donburi.Entry{}
    island.Tiles[buildings.KindForrestID] = []*donburi.Entry{}
    island.Tiles[buildings.KindRoadsID] = []*donburi.Entry{}
    
    // First pass: identify occupied cells
    occupiedByBuilding := make(map[[2]int]bool)
    occupiedByRoad := make(map[[2]int]bool)
    
    for y := 0; y < i.Height; y++ {
        for x := 0; x < i.Width; x++ {
            tile := i.Layers.Top.Get(x, y)
            if tile.Id == 0xFFFF { continue }
            
            building := i.Buildings.Buildings[tile.Id]
            if building.Kind.IsBuilding() {
                // Mark entire footprint
                for dy := 0; dy < building.Size.H; dy++ {
                    for dx := 0; dx < building.Size.W; dx++ {
                        occupiedByBuilding[[2]int{x+dx, y+dy}] = true
                    }
                }
            }
        }
    }
    
    // Second pass: create tile entities
    for y := 0; y < i.Height; y++ {
        for x := 0; x < i.Width; x++ {
            tile := i.Layers.Top.Get(x, y)
            if tile.Id == 0xFFFF { continue }
            
            // Create entity with components
            tileEntity := world.Create(BuildingType, PositionType, TileType, AnimationType)
            tileEntry := world.Entry(tileEntity)
            
            // Set components...
            
            // Assign to appropriate layer
            switch {
            case building.Kind.IsBuilding():
                island.Tiles[buildings.KindBuildingsID] = append(...)
            case building.Kind.IsWater():
                island.Tiles[buildings.KindSeaID] = append(...)
            // ... etc
            }
        }
    }
    
    IslandType.Set(islandEntry, island)
    return islandEntry
}
```

## Related Files

- `pkg/chunks/island5.go` - Island5 structure and Finalize()
- `pkg/chunks/island3.go` - Legacy format conversion
- `pkg/chunks/island4.go` - Legacy format conversion
- `pkg/chunks/islandhouse.go` - Tile grid structure
- `pkg/ecs/components/island.go` - ECS island component
- `pkg/gam/parser.go` - GAM file parsing
