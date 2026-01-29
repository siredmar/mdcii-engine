# COD File Format and Building Definitions

COD files are text-based configuration files used by Anno 1602 to define buildings, objects, and game constants. The main file is `haeuser.cod` which contains all building definitions.

## File Encoding

COD files are XOR-encoded for basic obfuscation:

```go
// pkg/cod/cod.go - NewCod()
if decode {
    for i, v := range b {
        b[i] = -v  // XOR with -1 (equivalent to ^0xFF)
    }
}
```

After decoding, the content is plain text with a custom syntax.

## Syntax Overview

### Constants

```
GFXBODEN = 0
GFXWALD = 20
TIMENEVER = -1
MAX_BUILDINGS = 500
```

### Variables (Object Properties)

```
Id:         20101
Gfx:        GFXBODEN+80
Size:       2, 3
Kind:       GEBAEUDE
Rotate:     4
AnimAnz:    8
AnimAdd:    1
```

### Arrays

```
Wegspeed:   0, 0, 0, 0
Kosten:     100, 50, 20
Maxware:    10, 20
```

### Relative Assignments

```
@Nummer:    +1          ; Increment from previous
@Gfx:       +4          ; Add 4 to previous Gfx value
@Pos:       +0, +42     ; Relative array assignment
```

### Object Hierarchy

```
Objekt: HAUS
    Nummer: 0
    
    Objekt: GEBAEUDE
        Id:     20101
        Gfx:    100
        Size:   2, 2
        
        Objekt: HAUS_PRODTYP
            Kind:   HANDWERK
            Ware:   WERKZEUG
        EndObj
        
        Objekt: HAUS_BAUKOST
            Money:  500
            Holz:   10
        EndObj
    EndObj
EndObj
```

### Includes

```
Include: "include/objekte.inc"
```

## Parser Implementation

The parser in `pkg/cod/parser.go` processes lines sequentially, handling each syntax type:

```go
func (c *Cod) Parse() error {
    for linenumber, rawLine := range c.Lines {
        line := strings.ReplaceAll(rawLine.Line, " ", "")
        
        // Try each handler in order of specificity
        if ok, _ := c.handleInclude(line, linenumber); ok { continue }
        if ok, _ := c.handleConstants(line); ok { continue }
        if ok, _ := c.handleVariableRelativeArray(line); ok { continue }
        if ok, _ := c.handleArray(line); ok { continue }
        if ok, _ := c.handleVariableRelative(line); ok { continue }
        if ok, _ := c.handleVariableWithConstant(line); ok { continue }
        if ok, _ := c.handleVariable(line, spaces); ok { continue }
        if ok, _ := c.handleObjects(line, spaces); ok { continue }
        if ok, _ := c.handleEndObjects(line, spaces); ok { continue }
        if ok, _ := c.handleObjFill(line, spaces); ok { continue }
        if ok, _ := c.handleNumberObject(line, spaces); ok { continue }
    }
    return nil
}
```

## Building Structure

After parsing, buildings are extracted from the "HAUS" object hierarchy:

```go
// pkg/cod/buildings/buildings.go
type Building struct {
    // Identity
    Id              int           // Unique ID (offset from 20000)
    Kind            Kind          // Category (Ground, Building, Sea, etc.)
    
    // Graphics
    Gfx             int           // Base sprite index in BSH
    Size            BuildingSize  // Footprint {W, H} in tiles
    Rotate          int           // Sprites per rotation (0 = symmetric)
    PositionOffset  int           // Vertical offset for elevation
    
    // Animation
    AnimationAmount int           // Number of animation frames
    AnimationAdd    int           // Sprite index increment per frame
    AnimationTime   int           // Animation speed (TIMENEVER = static)
    AnimationFrame  int           // Starting frame
    
    // Gameplay
    Block           int           // Blocking behavior
    MaxEnergy       int           // Hit points
    MaxFire         int           // Fire resistance
    PlaceFlag       int           // Placement rules
    
    // Production (for production buildings)
    HouseProduction HouseProductionType
    
    // Construction costs
    HouseBuildCosts HouseBuildCosts
}
```

## Key Building Fields Explained

### Id

Building IDs are stored with a 20000 offset in the COD file:
```go
func (b *Buildings) processId(value int) int {
    if value == 0 { return 0 }
    return value - b.IdOffset  // IdOffset = 20000
}
```
So `Id: 20101` in COD becomes `Id: 101` in code.

### Gfx (Graphics Index)

The base sprite index in the BSH file. Multi-tile buildings have consecutive sprites:
- 2x2 building at Gfx 100 uses sprites 100, 101, 102, 103

### Rotate

Number of sprites per rotation. Determines how rotated variants are stored:
- `Rotate: 0` - No rotation support (symmetric tile)
- `Rotate: 1` - 1 sprite per rotation (4 total for 4 rotations)
- `Rotate: 4` - 4 sprites per rotation (16 total for 2x2 building with 4 rotations)

**Sprite calculation for rotated buildings:**
```go
spriteIndex = Gfx + (rotationIndex * Rotate) + tileIndex
```

### AnimationAmount, AnimationAdd, AnimationTime

Animation parameters:
- `AnimationAmount`: Total animation frames
- `AnimationAdd`: Sprite offset per frame
- `AnimationTime`: Speed (frames per update, -1 = static)

**Animation frame calculation:**
```go
spriteIndex = Gfx + (animationFrame * AnimationAdd)
```

### Size

Building footprint in tiles:
```go
Size: 2, 3  // Width=2, Height=3
```

### Kind

Building category that determines rendering layer and behavior:

```go
// pkg/cod/buildings/types.go
const (
    KindGround    Kind = "Ground"    // Terrain tiles
    KindSea       Kind = "Sea"       // Water
    KindForrest   Kind = "Forrest"   // Trees
    KindStreet    Kind = "Street"    // Roads
    KindBuilding  Kind = "Building"  // Player structures
    KindWall      Kind = "Wall"      // Defensive walls
    KindHarbor    Kind = "Harbor"    // Port facilities
    // ... many more
)
```

### PositionOffset

Vertical elevation offset in pixels. Used for buildings on hills or multi-story structures.

## Tile Kind Categories

Kinds are grouped for rendering layer assignment:

```go
// Sea layer
KindsSea = []Kind{KindSea, KindEstuary, KindSurf}

// Ground layer
KindsGround = []Kind{
    KindGround, KindSlopeSpring, KindRock, KindSlope,
    KindBeach, KindBeachCornerI, KindBeachCornerII, ...
}

// Forest layer
KindsForrest = []Kind{KindForrest}

// Roads layer
KindsRoads = []Kind{KindStreet, KindBridge, KindPlaza}

// Buildings layer
KindBuildings = []Kind{
    KindGate, KindRuin, KindHeadquarters, KindBeachHouse,
    KindTower, KindMine, KindBuilding, KindWMill, KindWall, ...
}
```

## Production Types

For production buildings, nested HAUS_PRODTYP object defines:

```go
type HouseProductionType struct {
    Kind       Kind        // Production category
    Ware       Ware        // Output good
    Rohstoff   RawMaterial // Input material
    Radius     int         // Work radius
    Arbeiter   int         // Workers needed
    Prodmenge  int         // Production amount
    Interval   int         // Production interval
    Maxlager   int         // Storage capacity
    // ... more fields
}
```

## Build Costs

Construction requirements in HAUS_BAUKOST:

```go
type HouseBuildCosts struct {
    Money  int  // Gold coins
    Tools  int  // Werkzeug
    Wood   int  // Holz
    Stone  int  // Ziegel (bricks)
    Canons int  // Kanonen
}
```

## Usage in Codebase

### Loading Buildings

```go
import (
    "github.com/siredmar/mdcii-engine/pkg/cod"
    "github.com/siredmar/mdcii-engine/pkg/cod/buildings"
)

// Parse COD file
codPath := "path/to/haeuser.cod"
haeuserCod, err := cod.NewCod(codPath, true)  // true = decode
err = haeuserCod.Parse()

// Generate building definitions
buildingDefs, err := buildings.NewBuildings(haeuserCod)

// Access buildings
building := buildingDefs.Buildings[101]        // By ID
building := buildingDefs.BuildingsVector[0]    // By index
```

### Querying Buildings

```go
// Get building by ID
b, err := buildingDefs.GetBuilding(101)

// Get ID by index
id, err := buildingDefs.GetBuildingIdByIndex(50)

// Check properties
if b.Kind.IsBuilding() { /* ... */ }
if b.Kind.IsWater() { /* ... */ }
if b.IsRotatable() { /* ... */ }
```

## Important IDs

| ID Range | Category |
|----------|----------|
| 0-100 | Terrain/Ground tiles |
| 100-500 | Basic buildings |
| 500-800 | Advanced buildings |
| 1200-1210 | Sea/water tiles |
| 1201 | Deep sea (used for background) |

## Related Files

- `pkg/cod/cod.go` - COD file loading and structure
- `pkg/cod/parser.go` - Line-by-line parsing logic
- `pkg/cod/buildings/buildings.go` - Building struct and generation
- `pkg/cod/buildings/types.go` - Kind/Ware/Material enums and mappings
- `proto/cod.proto` - Protobuf definitions for Variables/Objects
