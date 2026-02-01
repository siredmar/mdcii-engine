package savegame

import (
	"time"

	"github.com/siredmar/mdcii-engine/pkg/chunks"
	"github.com/siredmar/mdcii-engine/pkg/cod/buildings"
	"github.com/siredmar/mdcii-engine/pkg/gam"
)

// ConvertFromGAM converts a parsed GAM file to the new savegame format
func ConvertFromGAM(parser *gam.GamParser, b *buildings.Buildings) (*Savegame, error) {
	s := &Savegame{
		Version: CurrentVersion,
		Metadata: Metadata{
			CreatedAt:  time.Now(),
			GameTick:   0,
			Extensions: make(map[string]any),
		},
		Meta: SavegameMeta{
			Camera: Camera{
				X:        0,
				Y:        0,
				Zoom:     1.0,
				Rotation: 0,
			},
		},
		World: World{
			Width:   500, // Default world dimensions
			Height:  350,
			Islands: make([]Island, 0, len(parser.Islands5)),
		},
	}

	for _, island5 := range parser.Islands5 {
		island, err := convertIsland5(island5, b)
		if err != nil {
			return nil, err
		}
		s.World.Islands = append(s.World.Islands, *island)
	}

	// Center camera on first island if available
	if len(s.World.Islands) > 0 {
		first := s.World.Islands[0]
		islandX := float64(first.Position.X)
		islandY := float64(first.Position.Y)
		// Local center of island
		lx := float64(first.Dimensions.Width) / 2
		ly := float64(first.Dimensions.Height) / 2
		// Renderer formula: origin = local_iso + island_offset
		tileWidth, tileHeight := 64.0, 32.0
		originX := (lx-ly)*(tileWidth/2) + islandX*(tileWidth/2)
		originY := (lx+ly)*(tileHeight/2) + islandY*(tileHeight/2)
		// Center on screen (assuming 1024x1024)
		s.Meta.Camera.X = originX - 512
		s.Meta.Camera.Y = originY - 512
	}

	return s, nil
}

// convertIsland5 converts a single Island5 chunk to the new Island format
func convertIsland5(i5 *chunks.Island5, b *buildings.Buildings) (*Island, error) {
	island := &Island{
		ID: i5.IslandNumber,
		Position: GridPosition{
			X: i5.Posx,
			Y: i5.Posy,
		},
		Dimensions: Dimensions{
			Width:  i5.Width,
			Height: i5.Height,
		},
		Climate:   convertClimate(i5.Climate),
		Fertility: convertFertility(i5.Fertility),
		Layers:    IslandLayers{},
	}

	// Classify tiles by layer
	if err := classifyTilesByLayer(i5, b, &island.Layers); err != nil {
		return nil, err
	}

	return island, nil
}

// convertClimate converts IslandClimate to Climate string
func convertClimate(c chunks.IslandClimate) Climate {
	switch c {
	case chunks.SouthClimate:
		return ClimateSouth
	default:
		return ClimateNorth
	}
}

// convertFertility converts Fertility bitfield to string slice
func convertFertility(f chunks.Fertility) []string {
	var result []string

	// Check fertility bits
	if f&0x0002 != 0 {
		result = append(result, "tobacco")
	}
	if f&0x0004 != 0 {
		result = append(result, "spices")
	}
	if f&0x0008 != 0 {
		result = append(result, "sugar")
	}
	if f&0x0010 != 0 {
		result = append(result, "wool")
	}
	if f&0x0020 != 0 {
		result = append(result, "wine")
	}
	if f&0x0040 != 0 {
		result = append(result, "cocoa")
	}

	return result
}

// classifyTilesByLayer iterates through island tiles and assigns to layers
func classifyTilesByLayer(i5 *chunks.Island5, b *buildings.Buildings, layers *IslandLayers) error {
	if i5.Layers.Top == nil {
		return nil
	}

	// Initialize layers
	layers.Sea = &TileLayer{Tiles: []Tile{}}
	layers.Ground = &TileLayer{Tiles: []Tile{}}
	layers.Roads = &TileLayer{Tiles: []Tile{}}
	layers.Forest = &TileLayer{Tiles: []Tile{}}
	layers.Buildings = &BuildingLayer{Entities: []BuildingEntity{}}

	// Track which positions are occupied by multi-tile buildings to avoid duplicates
	occupiedByBuilding := make(map[[2]int]bool)

	for y := 0; y < i5.Height; y++ {
		for x := 0; x < i5.Width; x++ {
			field := i5.Layers.Top.Get(x, y)
			if field.Id == 0xFFFF {
				continue
			}

			building := b.Buildings[field.Id]
			if building == nil {
				continue
			}

			// Create animation state from field
			animation := Animation{
				Frame:      field.AnimationCount,
				TimeOffset: 0,
				Running:    building.AnimationAmount > 0,
				Loop:       true,
			}

			switch {
			case building.Kind.IsWater():
				layers.Sea.Tiles = append(layers.Sea.Tiles, Tile{
					ID:        field.Id,
					Position:  GridPosition{X: x, Y: y},
					Rotation:  field.Orientation,
					Animation: &animation,
				})

			case building.Kind.IsGround():
				layers.Ground.Tiles = append(layers.Ground.Tiles, Tile{
					ID:        field.Id,
					Position:  GridPosition{X: x, Y: y},
					Rotation:  field.Orientation,
					Animation: &animation,
				})

			case building.Kind.IsRoad():
				layers.Roads.Tiles = append(layers.Roads.Tiles, Tile{
					ID:        field.Id,
					Position:  GridPosition{X: x, Y: y},
					Rotation:  field.Orientation,
					Animation: &animation,
				})

			case building.Kind.IsForrest():
				layers.Forest.Tiles = append(layers.Forest.Tiles, Tile{
					ID:        field.Id,
					Position:  GridPosition{X: x, Y: y},
					Rotation:  field.Orientation,
					Animation: &animation,
				})

			case building.Kind.IsBuilding():
				// Only add building entity at its origin (where Posx/Posy match x/y)
				// This avoids adding duplicate entities for multi-tile buildings
				posKey := [2]int{x, y}
				if occupiedByBuilding[posKey] {
					continue
				}

				// Mark footprint as occupied
				for dy := 0; dy < building.Size.H; dy++ {
					for dx := 0; dx < building.Size.W; dx++ {
						occupiedByBuilding[[2]int{x + dx, y + dy}] = true
					}
				}

				layers.Buildings.Entities = append(layers.Buildings.Entities, BuildingEntity{
					ID:        field.Id,
					Position:  GridPosition{X: x, Y: y},
					Rotation:  field.Orientation,
					Owner:     field.PlayerNumber,
					City:      field.CityNumber,
					Animation: animation,
				})
			}
		}
	}

	return nil
}
