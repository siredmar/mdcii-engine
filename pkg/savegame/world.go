package savegame

import (
	"github.com/siredmar/mdcii-engine/pkg/building"
	"github.com/siredmar/mdcii-engine/pkg/cod/buildings"
	"github.com/siredmar/mdcii-engine/pkg/ecs/components"
	animation "github.com/siredmar/mdcii-engine/pkg/texture/animations"
	"github.com/siredmar/mdcii-engine/pkg/world/rotation"
	"github.com/siredmar/mdcii-engine/pkg/world/zoom"
	"github.com/yohamta/donburi"
)

// ToECSWorld creates ECS entities from the savegame data and returns the world entity
func (s *Savegame) ToECSWorld(w donburi.World, ani *animation.Animations, b *buildings.Buildings) (*donburi.Entry, error) {
	// Create world entity
	worldEntity := w.Create(components.WorldType)
	worldEntry := w.Entry(worldEntity)

	ecsWorld := &components.World{
		Width:   s.World.Width,
		Height:  s.World.Height,
		Islands: make([]*donburi.Entry, 0, len(s.World.Islands)),
	}

	// Create island entities
	for _, island := range s.World.Islands {
		islandEntry := island.ToECSEntity(w, ani, b)
		ecsWorld.Islands = append(ecsWorld.Islands, islandEntry)
	}

	components.WorldType.Set(worldEntry, ecsWorld)
	return worldEntry, nil
}

// CreateCameraEntity creates and returns a camera entity from the savegame camera state.
// If camera was not initialized (never moved by user), centers on the world center.
// Camera position is stored in tile grid coordinates.
func (s *Savegame) CreateCameraEntity(w donburi.World) *donburi.Entry {
	cameraEntity := w.Create(components.CameraType)
	cameraEntry := w.Entry(cameraEntity)

	cam := s.Meta.Camera

	// If camera was never initialized by user, center on world
	if !cam.Initialized {
		// Center on world - camera position is in tile coordinates
		// Position camera at the center of the world
		// Screen center offset in tile units (for 1024x1024 screen)
		const screenCenterTileX = 24.0
		const screenCenterTileY = 8.0
		cam.X = float64(s.World.Width)/2 - screenCenterTileX
		cam.Y = float64(s.World.Height)/2 - screenCenterTileY
		cam.Zoom = 1.0
		// Don't mark as initialized yet - will be marked when user moves camera
	}

	// Default zoom to 1.0 if not set
	if cam.Zoom <= 0 {
		cam.Zoom = 1.0
	}

	components.CameraType.Set(cameraEntry, &components.Camera{
		X:        cam.X,
		Y:        cam.Y,
		Zoom:     cam.Zoom,
		Rotation: rotation.Rotation(cam.Rotation),
	})

	return cameraEntry
}

// ToECSEntity creates an ECS island entity from the Island data
func (island *Island) ToECSEntity(w donburi.World, ani *animation.Animations, b *buildings.Buildings) *donburi.Entry {
	// Create the island entity
	islandEntity := w.Create(components.IslandType)
	islandEntry := w.Entry(islandEntity)

	// Initialize the Island component
	ecsIsland := &components.Island{
		Width:   island.Dimensions.Width,
		Height:  island.Dimensions.Height,
		X:       float64(island.Position.X),
		Y:       float64(island.Position.Y),
		Climate: convertToECSClimate(island.Climate),
		Tiles:   map[string][]*donburi.Entry{},
	}

	// Initialize layer maps
	ecsIsland.Tiles[buildings.KindBuildingsID] = []*donburi.Entry{}
	ecsIsland.Tiles[buildings.KindSeaID] = []*donburi.Entry{}
	ecsIsland.Tiles[buildings.KindSeaID+"_OVERLAY"] = []*donburi.Entry{}
	ecsIsland.Tiles[buildings.KindRoadsID] = []*donburi.Entry{}
	ecsIsland.Tiles[buildings.KindGroundID] = []*donburi.Entry{}
	ecsIsland.Tiles[buildings.KindGroundID+"_OVERLAY"] = []*donburi.Entry{}
	ecsIsland.Tiles[buildings.KindForrestID] = []*donburi.Entry{}

	worldX := float64(island.Position.X)
	worldY := float64(island.Position.Y)

	// Process Sea tiles
	if island.Layers.Sea != nil {
		for _, tile := range island.Layers.Sea.Tiles {
			entry := createTileEntity(w, ani, b, tile, worldX, worldY)
			ecsIsland.Tiles[buildings.KindSeaID] = append(ecsIsland.Tiles[buildings.KindSeaID], entry)
		}
	}

	// Process Ground tiles
	if island.Layers.Ground != nil {
		for _, tile := range island.Layers.Ground.Tiles {
			entry := createTileEntity(w, ani, b, tile, worldX, worldY)
			ecsIsland.Tiles[buildings.KindGroundID] = append(ecsIsland.Tiles[buildings.KindGroundID], entry)
		}
	}

	// Process Roads tiles
	if island.Layers.Roads != nil {
		for _, tile := range island.Layers.Roads.Tiles {
			entry := createTileEntity(w, ani, b, tile, worldX, worldY)
			ecsIsland.Tiles[buildings.KindRoadsID] = append(ecsIsland.Tiles[buildings.KindRoadsID], entry)
		}
	}

	// Process Forest tiles
	if island.Layers.Forest != nil {
		for _, tile := range island.Layers.Forest.Tiles {
			entry := createTileEntity(w, ani, b, tile, worldX, worldY)
			ecsIsland.Tiles[buildings.KindForrestID] = append(ecsIsland.Tiles[buildings.KindForrestID], entry)
		}
	}

	// Process Building entities
	if island.Layers.Buildings != nil {
		for _, entity := range island.Layers.Buildings.Entities {
			entries := createBuildingEntities(w, ani, b, entity, worldX, worldY)
			ecsIsland.Tiles[buildings.KindBuildingsID] = append(ecsIsland.Tiles[buildings.KindBuildingsID], entries...)
		}
	}

	components.IslandType.Set(islandEntry, ecsIsland)
	return islandEntry
}

// createTileEntity creates a single tile ECS entity
func createTileEntity(w donburi.World, ani *animation.Animations, b *buildings.Buildings, tile Tile, worldX, worldY float64) *donburi.Entry {
	tileEntity := w.Create(components.BuildingType, components.PositionType, components.TileType, components.AnimationType)
	tileEntry := w.Entry(tileEntity)

	tileB := b.Buildings[tile.ID]
	if tileB == nil {
		return tileEntry
	}

	posOffset := tileB.PositionOffset
	size := tileB.Size

	components.PositionType.Set(tileEntry, &components.Position{
		X:      worldX + float64(tile.Position.X),
		Y:      worldY + float64(tile.Position.Y),
		Offset: float64(posOffset),
	})

	// For tiles with Rotate=0 in COD, apply runtime sprite rotation
	spriteRot := 0
	if tileB.Rotate == 0 && tile.Rotation > 0 && !tileB.Kind.IsWater() && !tileB.Kind.IsForrest() {
		spriteRot = tile.Rotation
	}

	components.TileType.Set(tileEntry, &components.Tile{
		Size:           components.Size{Width: size.W, Height: size.H, Z: size.H - zoom.TileHeight()},
		SpriteRotation: spriteRot,
	})

	anim := ani.GetAnimation(tile.ID, rotation.Rotation(tile.Rotation))

	// Use animation state from savegame
	ecsAnim := &components.Animation{
		Count:        anim.Steps,
		Duration:     float64(anim.FrameDuration),
		Running:      true,
		Loop:         true,
		CurrentFrame: 0,
		CurrentTime:  0,
	}
	if tile.Animation != nil {
		ecsAnim.CurrentFrame = tile.Animation.Frame
		ecsAnim.CurrentTime = tile.Animation.TimeOffset
		ecsAnim.Running = tile.Animation.Running
		ecsAnim.Loop = tile.Animation.Loop
	}
	components.AnimationType.Set(tileEntry, ecsAnim)

	components.BuildingType.Set(tileEntry, &components.Building{
		BuildingID: tile.ID,
		Rotation:   rotation.Rotation(tile.Rotation),
		Size:       building.BuildingSize(size.W, size.H),
	})

	return tileEntry
}

// createBuildingEntities creates ECS entities for a building (origin + occupation markers)
func createBuildingEntities(w donburi.World, ani *animation.Animations, b *buildings.Buildings, entity BuildingEntity, worldX, worldY float64) []*donburi.Entry {
	var entries []*donburi.Entry

	tileB := b.Buildings[entity.ID]
	if tileB == nil {
		return entries
	}

	posOffset := tileB.PositionOffset
	size := tileB.Size

	// Create the main building entity
	buildingEntity := w.Create(components.BuildingType, components.PositionType, components.TileType, components.AnimationType)
	buildingEntry := w.Entry(buildingEntity)

	components.PositionType.Set(buildingEntry, &components.Position{
		X:      worldX + float64(entity.Position.X),
		Y:      worldY + float64(entity.Position.Y),
		Offset: float64(posOffset),
	})

	spriteRot := 0
	if tileB.Rotate == 0 && entity.Rotation > 0 && !tileB.Kind.IsWater() && !tileB.Kind.IsForrest() {
		spriteRot = entity.Rotation
	}

	components.TileType.Set(buildingEntry, &components.Tile{
		Size:           components.Size{Width: size.W, Height: size.H, Z: size.H - zoom.TileHeight()},
		SpriteRotation: spriteRot,
	})

	anim := ani.GetAnimation(entity.ID, rotation.Rotation(entity.Rotation))
	components.AnimationType.Set(buildingEntry, &components.Animation{
		Count:        anim.Steps,
		Duration:     float64(anim.FrameDuration),
		Running:      entity.Animation.Running,
		Loop:         entity.Animation.Loop,
		CurrentFrame: entity.Animation.Frame,
		CurrentTime:  entity.Animation.TimeOffset,
	})

	components.BuildingType.Set(buildingEntry, &components.Building{
		BuildingID: entity.ID,
		Rotation:   rotation.Rotation(entity.Rotation),
		Size:       building.BuildingSize(size.W, size.H),
	})

	entries = append(entries, buildingEntry)

	// Create occupation markers for the building footprint
	for dy := 0; dy < size.H; dy++ {
		for dx := 0; dx < size.W; dx++ {
			occupyEntity := w.Create(components.BuildingType, components.PositionType, components.TileType, components.AnimationType)
			occupyEntry := w.Entry(occupyEntity)

			components.BuildingType.Set(occupyEntry, &components.Building{
				BuildingID: -1,
				Rotation:   0,
				Size:       building.BuildingSize(1, 1),
			})
			components.AnimationType.Set(occupyEntry, &components.Animation{Count: 1, Duration: 1})
			components.PositionType.Set(occupyEntry, &components.Position{
				X:      worldX + float64(entity.Position.X+dx),
				Y:      worldY + float64(entity.Position.Y+dy),
				Offset: float64(posOffset),
			})
			components.TileType.Set(occupyEntry, &components.Tile{
				Size:       components.Size{Width: 1, Height: 1, Z: size.H - zoom.TileHeight()},
				Image:      nil,
				Occupation: true,
			})

			entries = append(entries, occupyEntry)
		}
	}

	return entries
}

func convertToECSClimate(c Climate) components.Climate {
	switch c {
	case ClimateSouth:
		return components.South
	default:
		return components.North
	}
}
