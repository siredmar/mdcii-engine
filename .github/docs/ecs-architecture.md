# ECS Architecture

The engine uses the [donburi](https://github.com/yohamta/donburi) ECS (Entity-Component-System) library to structure game data and logic.

## Core Concepts

### World

The `donburi.World` is the central container for all entities and components:

```go
import "github.com/yohamta/donburi"

world := donburi.NewWorld()
```

### Entities

Entities are unique identifiers that group components together:

```go
// Create entity with specific components
entity := world.Create(BuildingType, PositionType, TileType, AnimationType)
entry := world.Entry(entity)
```

### Components

Components are pure data containers. Each is defined as a typed pointer:

```go
// Definition
var BuildingType = donburi.NewComponentType[Building]()

// Access
building := components.BuildingType.Get(entry)
building.Rotation = rotation.DEG90

// Set
components.BuildingType.Set(entry, &Building{BuildingID: 101})
```

### Queries

Queries filter entities by component composition:

```go
// Define query
var rendererQuery = donburi.NewQuery(
    filter.Contains(components.IslandType),
)

// Execute
rendererQuery.Each(world, func(entry *donburi.Entry) {
    island := components.IslandType.Get(entry)
    // Process island
})
```

## Components Reference

### Position

Grid coordinates within the world:

```go
// pkg/ecs/components/position.go
type Position struct {
    X, Y   float64  // Grid position (can include island offset)
    Offset float64  // Vertical offset for elevated tiles
}

var PositionType = donburi.NewComponentType[Position]()
```

**Usage:**
- X, Y are world grid coordinates (island.X + local.X)
- Offset raises the sprite vertically (for hills, platforms)

### Building

Building/tile type identity:

```go
// pkg/ecs/components/building.go
type Building struct {
    BuildingID int                             // COD building ID (e.g., 101 = house)
    Rotation   rotation.Rotation               // Current rotation (0-3)
    Size       building.BuildingSizeIdentifier // 1x1, 2x2, etc.
}

var BuildingType = donburi.NewComponentType[Building]()
```

**Special values:**
- `BuildingID == -1`: Placeholder entity for building footprint occupation
- `BuildingID == 0xFFFF`: Empty/invalid

### Tile

Rendering data for a tile:

```go
// pkg/ecs/components/tile.go
type Tile struct {
    Image          *ebiten.Image  // Current sprite frame
    PivotX         int            // Anchor X offset for rendering
    PivotY         int            // Anchor Y offset for rendering
    Size           Size           // Tile dimensions
    Occupation     bool           // True if this marks footprint occupation
    SpriteRotation int            // 0-3: runtime 90° rotations (for Rotate=0 tiles)
}

type Size struct {
    Width  int
    Height int
    Z      int  // Vertical height offset
}

var TileType = donburi.NewComponentType[Tile]()
```

**Pivot interpretation:**
- Drawing position: `screenX = originX - PivotX`
- See texture-atlas.md for pivot calculation

### Animation

Animation state for animated tiles:

```go
// pkg/ecs/components/animation.go
type Animation struct {
    CurrentFrame int      // Current frame index (0 to Count-1)
    Count        int      // Total number of frames
    Duration     float64  // Total animation duration in seconds
    CurrentTime  float64  // Elapsed time accumulator
    Loop         bool     // Whether to loop animation
    Running      bool     // Whether animation is playing
    Reset        bool     // Flag to reset animation
    Next         bool     // (Reserved)
}

var AnimationType = donburi.NewComponentType[Animation]()
```

### Camera

Viewport position and state:

```go
// pkg/ecs/components/camera.go
type Camera struct {
    X        float64            // World X offset (scroll position)
    Y        float64            // World Y offset
    Zoom     float64            // Zoom level (unused currently)
    Rotation rotation.Rotation  // Camera rotation (unused, world rotates instead)
}

var CameraType = donburi.NewComponentType[Camera]()
```

**Scrolling:**
- Tiles render at `screenX = isoX - camera.X`
- Camera moves via WASD/arrow keys or mouse drag

### Control

Global input and view state:

```go
// pkg/ecs/components/control.go
type Control struct {
    Rotation         rotation.Rotation               // Current world rotation (Q/E keys)
    GridVisible      bool                            // Debug grid overlay (G key)
    OverlayVisible   bool                            // Show tile origins (O key)
    LastKeyPressTime time.Time                       // For debouncing
    
    Dragging         bool                            // Mouse drag state
    LastMouseX       int                             // Previous mouse X
    LastMouseY       int                             // Previous mouse Y
    
    SelectedSize     building.BuildingSizeIdentifier // For debug
}

var ControlType = donburi.NewComponentType[Control]()
```

### Island

Container for all tiles on an island:

```go
// pkg/ecs/components/island.go
type Island struct {
    Width, Height int                         // Grid dimensions
    X, Y          float64                     // World position
    Climate       Climate                     // North(0) or South(1)
    Tiles         map[string][]*donburi.Entry // Layer → tile entities
}

type Climate int
const (
    North Climate = iota
    South
    Any
)

var IslandType = donburi.NewComponentType[Island]()
```

**Layer keys:**
- `buildings.KindBuildingsID` ("Buildings")
- `buildings.KindSeaID` ("Sea")
- `buildings.KindGroundID` ("Ground")
- `buildings.KindForrestID` ("Forrest")
- `buildings.KindRoadsID` ("Roads")
- `*_OVERLAY` variants for base layer underlays

## Systems Reference

### AnimationSystem

Updates animation frames based on elapsed time:

```go
// pkg/ecs/systems/animation.go
func AnimationSystem(world donburi.World, ani *animations.Animations, deltaTime float64)
```

**Query:** Entities with Building + Animation + Tile

**Logic:**
1. Get current world rotation from Control component
2. For each animated entity:
   - Combine building rotation with world rotation
   - Calculate frame index from elapsed time
   - Update Tile.Image with current frame
   - Update pivot points for stable positioning

```go
frameDuration := animation.Duration / float64(len(frames))
frameIndex := int(animation.CurrentTime / frameDuration)
if animation.Loop {
    frameIndex %= len(frames)
}
tile.Image = frames[frameIndex]
```

### InputSystem

Handles keyboard and mouse input:

```go
// pkg/ecs/systems/input.go
func InputSystem(world donburi.World)
```

**Controls:**
| Key | Action |
|-----|--------|
| Q | Rotate world clockwise |
| E | Rotate world counter-clockwise |
| G | Toggle debug grid |
| O | Toggle overlay (tile origins) |
| WASD/Arrows | Pan camera |
| Right-drag | Pan camera with mouse |
| Escape | Exit |

**Debouncing:** 150ms between key actions to prevent rapid toggling.

### RenderSystem

Draws all visible tiles:

```go
// pkg/ecs/systems/renderer.go
func RenderSystem(world donburi.World, screen *ebiten.Image, grid bool, currentRotation rotation.Rotation)
```

**Flow:**
1. Find camera and control state
2. Draw infinite sea background
3. Build map of occupied positions (buildings, roads)
4. Collect all visible tiles with screen positions
5. Sort by layer, then by depth (Y, X)
6. Draw sorted tiles with camera offset
7. (Optional) Draw debug overlay

See rendering.md for detailed rendering logic.

## Entity Creation

### Creating Islands from Savegame

The `CreateIslandFromChunk()` function converts parsed island data into ECS entities:

```go
// pkg/ecs/components/island.go
func CreateIslandFromChunk(world donburi.World, ani *animations.Animations, 
                           i *island5.Island5, worldX, worldY float64) *donburi.Entry
```

**Flow:**
1. Create Island entity
2. Initialize layer maps
3. **First pass:** Build occupation maps for building/road footprints
4. **Second pass:** For each tile in merged Top layer:
   - Create underlay entity if base differs from overlay
   - Create main tile entity with all components
   - Add special ground underlays for slopes/cliffs
   - Add occupation markers for building footprints
   - Assign to appropriate layer list

### Creating Control Entities

```go
// In cmd/island_ecs main setup
controlEntity := world.Create(components.ControlType)
controlEntry := world.Entry(controlEntity)
components.ControlType.Set(controlEntry, &components.Control{
    Rotation:    rotation.DEG0,
    GridVisible: false,
})
```

### Creating Camera Entity

```go
cameraEntity := world.Create(components.CameraType)
cameraEntry := world.Entry(cameraEntity)
components.CameraType.Set(cameraEntry, &components.Camera{
    X: 0, Y: 0, Zoom: 1.0,
})
```

## Query Patterns

### Find Single Entity

```go
cameraQuery := donburi.NewQuery(filter.Contains(components.CameraType))
cameraQuery.Each(world, func(entry *donburi.Entry) {
    camera = components.CameraType.Get(entry)
})
```

### Filter by Multiple Components

```go
var animationQuery = donburi.NewQuery(
    filter.Contains(components.BuildingType, components.AnimationType, components.TileType),
)
```

### Process Islands and Their Tiles

```go
rendererQuery.Each(world, func(entry *donburi.Entry) {
    island := components.IslandType.Get(entry)
    for _, tileEntry := range island.Tiles[buildings.KindBuildingsID] {
        pos := components.PositionType.Get(tileEntry)
        tile := components.TileType.Get(tileEntry)
        // ...
    }
})
```

## Game Loop Integration

The main game loop calls systems each frame:

```go
// In Ebiten's Update()
func (g *Game) Update() error {
    deltaTime := 1.0 / 60.0  // Assume 60 FPS
    
    systems.InputSystem(g.world)
    systems.AnimationSystem(g.world, g.animations, deltaTime)
    
    return nil
}

// In Ebiten's Draw()
func (g *Game) Draw(screen *ebiten.Image) {
    ctrl := getControl(g.world)
    systems.RenderSystem(g.world, screen, ctrl.GridVisible, ctrl.Rotation)
}
```

## Related Files

- `pkg/ecs/components/*.go` - All component definitions
- `pkg/ecs/systems/animation.go` - Animation system
- `pkg/ecs/systems/input.go` - Input handling
- `pkg/ecs/systems/renderer.go` - Rendering system
- `cmd/island_ecs/cmd/root.go` - Main game setup
