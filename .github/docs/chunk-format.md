# Chunk Format (GAM/SCP Files)

Anno 1602 savegames (.gam) and scenario files (.scp) use a binary chunk-based format. Each file consists of sequential chunks, where each chunk has a type identifier and payload data.

## Chunk Structure

### Header (20 bytes)

| Offset | Size | Field | Description |
|--------|------|-------|-------------|
| 0x00 | 16 | Id | Null-terminated ASCII chunk type (e.g., "INSEL5\0...") |
| 0x10 | 4 | Length | Payload length in bytes (little-endian) |

### Payload

Immediately follows the header, `Length` bytes of chunk-specific data.

```go
// pkg/chunks/chunk.go
const CHUNK_HEADER_OFFSET uint32 = 20

type Chunk struct {
    Id     string   // Chunk type identifier
    Length int      // Payload size
    Data   []byte   // Raw payload data
}
```

## Parsing Chunks

### Reading a Single Chunk

```go
func NewChunk(data []byte) (*Chunk, error) {
    chRaw := chunkRaw{}
    err := binstruct.UnmarshalLE(data, &chRaw)
    
    // Extract null-terminated string from 16-byte Id field
    id := extractNullTerminatedString(chRaw.Id[:])
    
    // Read payload data
    payload := data[CHUNK_HEADER_OFFSET : CHUNK_HEADER_OFFSET+chRaw.Length]
    
    return &Chunk{
        Id:     id,
        Length: int(chRaw.Length),
        Data:   payload,
    }, nil
}
```

### Reading All Chunks from File

```go
func ParseChunks(data []byte) ([]*Chunk, error) {
    var chunks []*Chunk
    currentIndex := 0
    
    for currentIndex < len(data) {
        chunk, err := NewChunk(data[currentIndex:])
        if err != nil {
            return nil, err
        }
        chunks = append(chunks, chunk)
        
        // Move to next chunk
        currentIndex += int(CHUNK_HEADER_OFFSET) + chunk.Length
    }
    
    return chunks, nil
}
```

## Common Chunk Types

### Island Chunks

| Chunk ID | Size | Description |
|----------|------|-------------|
| `INSEL5` | 116 bytes | Current island metadata format |
| `INSEL4` | ~112 bytes | Older island format (converted to INSEL5) |
| `INSEL3` | ~40 bytes | Oldest island format (converted to INSEL5) |
| `INSELHAUS` | Variable | Tile grid data for an island |

### Other Chunks

| Chunk ID | Description |
|----------|-------------|
| `HIRSCH2` | Deer/wildlife data |
| `STADT4` | City/settlement data |
| `KONTOR2` | Warehouse/trading post data |
| `SHIP4` | Ship data |
| `SOLDAT3` | Soldier/military unit data |
| `PRODLIST2` | Production queue data |
| `PLAYER4` | Player information |
| `TIMERS` | Game timer states |

## Chunk Ordering in GAM Files

Chunks appear in a specific order. Island-related chunks are grouped:

```
INSEL5 (or INSEL3/INSEL4)
    INSELHAUS          <- Tile data for this island
    [optional: INSELHAUS] <- Second layer (player modifications)
INSEL5
    INSELHAUS
    ...
HIRSCH2               <- Wildlife after islands
STADT4                <- Cities
...
```

**Critical**: `INSELHAUS` chunks immediately follow their parent island chunk. The parser tracks the "current island" to associate them correctly.

## GAM Parser Flow

```go
// pkg/gam/parser.go
func (p *GamParser) Parse(b *buildings.Buildings) error {
    chunksList, _ := chunks.ParseChunks(p.Data)
    
    for _, chunk := range chunksList {
        switch chunk.Id {
        case "INSEL5":
            island, _ := chunks.NewIsland5(chunk, b)
            p.Islands5 = append(p.Islands5, island)
            
        case "INSEL3":
            i3, _ := chunks.NewIsland3(chunk)
            p.Islands5 = append(p.Islands5, i3.ToIsland5())
            
        case "INSEL4":
            i4, _ := chunks.NewIsland4(chunk)
            p.Islands5 = append(p.Islands5, i4.ToIsland5())
            
        case "INSELHAUS":
            // Associate with most recent island
            currentIsland := p.Islands5[len(p.Islands5)-1]
            house, _ := chunks.NewIslandHouse(chunk, 
                chunks.IslandDimensions{
                    Width:  currentIsland.Width, 
                    Height: currentIsland.Height,
                }, b)
            currentIsland.AddIslandHouse(*house)
        }
    }
    
    // Finalize all islands (merge layers)
    for _, island := range p.Islands5 {
        island.Finalize()
    }
    
    return nil
}
```

## INSEL5 Structure (116 bytes)

The primary island metadata chunk:

```go
// pkg/chunks/island5.go
type island5Data struct {
    IslandNumber    int `bitfield:"8"`           // Unique island ID
    Width           int `bitfield:"8"`           // Grid width
    Height          int `bitfield:"8"`           // Grid height
    Strtduerrflg    int `bitfield:"1"`           // Drought start flag
    Nofixflg        int `bitfield:"1"`           // No-fix flag
    Vulkanflg       int `bitfield:"1"`           // Volcano flag
    Reserved0       int `bitfield:"5,reserved"`
    Posx            int `bitfield:"16"`          // World X position
    Posy            int `bitfield:"16"`          // World Y position
    Hirschreviercnt int `bitfield:"16"`          // Deer territory count
    Speedcnt        int `bitfield:"16"`          // Speed counter
    Stadtplayernr   [11]uint8                    // City-to-player mapping
    Vulkancnt       int `bitfield:"8"`           // Volcano counter
    Schatzflg       int `bitfield:"8"`           // Treasure flag
    Rohstanz        int `bitfield:"8"`           // Resource count
    Eisencnt        int `bitfield:"8"`           // Iron counter
    Playerflags     int `bitfield:"8"`           // Player visibility flags
    Eisenberg       [4]OreMountainData           // Iron deposits (4×8 bytes)
    Vulkanberg      [4]OreMountainData           // Volcano deposits (4×8 bytes)
    Fertility       Fertility `bitfield:"32"`    // Crop fertility flags
    FileNumber      int       `bitfield:"16"`    // Source .scp file number
    Size            IslandSize `bitfield:"16"`   // Size category
    Climate         IslandClimate `bitfield:"8"` // Climate zone
    ModifiedFlag    IslandModified `bitfield:"8"` // Has player changes
    Duerrproz       int `bitfield:"8"`           // Drought percentage
    Rotier          int `bitfield:"8"`           // Island rotation
    Seeplayerflags  int `bitfield:"32"`          // Sea player flags
    Duerrcnt        int `bitfield:"32"`          // Drought counter
    Reserved1       int `bitfield:"32,reserved"`
}
```

### Island Size Enum

```go
type IslandSize int
const (
    LittleIsland IslandSize = iota  // lit, sml
    MiddleIsland                     // mit, med
    BigIsland                        // gro, big
    HugeIsland                       // grs, lrg
)
```

### Island Climate Enum

```go
type IslandClimate int
const (
    North IslandClimate = iota  // Northern climate (NORDNAT/)
    South                        // Southern climate (SUEDNAT/)
)
```

### Island File Path

Unmodified islands reference a template .scp file:

```go
func (i *Island5) IslandFileName() string {
    // e.g., "NORDNAT/LIT02.SCP"
    return fmt.Sprintf("%s/%s%02d.scp", 
        IslandClimateMap[i.Climate],   // "NORDNAT" or "SUEDNAT"
        IslandSizeMap[i.Size],         // "LIT", "MIT", "GRO", "GRS"
        i.FileNumber)
}
```

## INSELHAUS Structure (8 bytes per field)

Contains the tile grid for an island. Each field is 8 bytes:

```go
// pkg/chunks/islandhouse.go
const IslandHouseFieldSize = 8

type Field struct {
    Id             int  // Building/tile ID (2 bytes, little-endian)
    Posx           int  // X position in island grid (1 byte)
    Posy           int  // Y position in island grid (1 byte)
    
    // Packed bitfield (4 bytes):
    Orientation    int  // bits 0-1:  rotation (0-3)
    AnimationCount int  // bits 2-5:  animation frame
    IslandNumber   int  // bits 6-13: island index
    CityNumber     int  // bits 14-16: city index
    RandomNumber   int  // bits 17-21: random seed
    PlayerNumber   int  // bits 22-25: owner player
    Reserved       int  // bits 26-31: reserved
}
```

### Binary Layout

```
Bytes 0-1: Id (uint16, little-endian)
Byte 2:    Posx
Byte 3:    Posy
Bytes 4-7: Packed bitfield (uint32, little-endian)
```

### Bitfield Extraction

```go
bits := uint32(fieldData[4]) | uint32(fieldData[5])<<8 | 
        uint32(fieldData[6])<<16 | uint32(fieldData[7])<<24

field.Orientation    = int((bits >> 0) & 0x03)   // 2 bits
field.AnimationCount = int((bits >> 2) & 0x0F)   // 4 bits
field.IslandNumber   = int((bits >> 6) & 0xFF)   // 8 bits
field.CityNumber     = int((bits >> 14) & 0x07)  // 3 bits
field.RandomNumber   = int((bits >> 17) & 0x1F)  // 5 bits
field.PlayerNumber   = int((bits >> 22) & 0x0F)  // 4 bits
field.Reserved       = int((bits >> 26) & 0x3F)  // 6 bits
```

### Special ID Values

| ID Value | Meaning |
|----------|---------|
| `0xFFFF` (65535) | Empty/transparent tile (use base layer) |
| `102` | Remapped to `169` (historical bug fix) |

## Finding Chunks by ID

```go
func FindChunkById(chunks []*Chunk, id string) (*Chunk, error) {
    for _, chunk := range chunks {
        if chunk.Id == id {
            return chunk, nil
        }
    }
    return nil, errors.New("chunk not found")
}
```

## Usage Example

```go
import (
    "github.com/siredmar/mdcii-engine/pkg/gam"
    "github.com/siredmar/mdcii-engine/pkg/cod/buildings"
)

// Load buildings definition first
buildings, _ := loadBuildings()

// Create parser
parser, _ := gam.NewParser()

// Load savegame
parser.LoadPath("SAVEGAME/lastgame.gam")

// Parse all chunks
parser.Parse(buildings)

// Access islands
for _, island := range parser.Islands5 {
    fmt.Printf("Island at (%d, %d), size %dx%d\n",
        island.Posx, island.Posy, 
        island.Width, island.Height)
    
    // Access tile grid
    for y := 0; y < island.Height; y++ {
        for x := 0; x < island.Width; x++ {
            tile := island.Layers.Top.Get(x, y)
            if tile.Id != 0xFFFF {
                fmt.Printf("Tile at (%d,%d): ID=%d\n", x, y, tile.Id)
            }
        }
    }
}
```

## Related Files

- `pkg/chunks/chunk.go` - Chunk parsing primitives
- `pkg/chunks/island5.go` - INSEL5 structure and layer merging
- `pkg/chunks/island3.go` - Legacy INSEL3 format
- `pkg/chunks/island4.go` - Legacy INSEL4 format
- `pkg/chunks/islandhouse.go` - INSELHAUS tile grid parsing
- `pkg/gam/parser.go` - High-level GAM file parser
