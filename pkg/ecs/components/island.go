package components

import (
	"encoding/json"
	"fmt"

	"github.com/siredmar/mdcii-engine/pkg/building"
	island5 "github.com/siredmar/mdcii-engine/pkg/chunks"
	"github.com/siredmar/mdcii-engine/pkg/cod/buildings"
	"github.com/siredmar/mdcii-engine/pkg/world/rotation"
	"github.com/yohamta/donburi"

	animations "github.com/siredmar/mdcii-engine/pkg/texture/animations"
)

type Island struct {
	Width, Height int                         // Dimensions of the island
	X, Y          float64                     // Position of the island in the world
	Climate       Climate                     `json:"climate"`
	Layers        map[string][]*donburi.Entry // Tiles grouped by layer
}

var IslandType = donburi.NewComponentType[Island]()

type Climate int

const (
	North Climate = iota
	South
	Any
)

// type Option func(*Island)

// func WithChunk(gfxSprites *sprites.Sprites, b *buildings.Buildings, c *chunks.Island5) Option {
// 	return func(*Island) {
// 		i := &Island{}

// 		i.Width = c.Width
// 		i.Height = c.Height
// 		i.Number = c.IslandNumber
// 		i.Climate = Climate(c.Climate)
// 		i.Terrain = make([]*tiles.TerrainTile, i.Width*i.Height)

// 		for y := range i.Height {
// 			// fmt.Println("")
// 			for x := range i.Width {
// 				t := c.Layers.Top.Fields[y*i.Width+x]
// 				if t.Id == 65535 {
// 					//  || t.Id == 102 {
// 					continue
// 				}
// 				if x != t.Posx || y != t.Posy {
// 					fmt.Printf("Error: x: %d, y: %d, PosX: %d, PosY: %d\n", x, y, t.Posx, t.Posy)
// 				}
// 				// fmt.Printf("%d ", rotation.Rotation(t.Orientation))
// 				// t := g.gam.Islands5[0].Layers.Top.Fields[y*g.gam.Islands5[0].Width+x]
// 				// // 		if t.Id == 65535 || t.Id == 102 {
// 				// // 			continue
// 				// // 		}
// 				// // 		if t.Id == 1201 {
// 				// // 			fmt.Println("found 1201")
// 				// // 		}
// 				// // 		building := g.buildings.Buildings[t.Id]

// 				building := b.Buildings[t.Id]
// 				tile := tiles.NewTerrainTile(rotation.Rotation(t.Orientation), x, y, building, gfxSprites)
// 				// if building.Kind.IsWater() {
// 				// 	i.Water = append(i.Water, tile)
// 				// 	continue
// 				// }
// 				// if building.Kind.IsCoast() {
// 				// 	i.Coast = append(i.Coast, tile)
// 				// 	continue
// 				// }

// 				i.Terrain[x+y*i.Width] = tile
// 				// i.Terrain = append(i.Terrain, tile)
// 			}
// 		}
// 	}
// }

// func NewIsland(opts ...Option) (*Island, error) {
// 	i := &Island{
// 		Version: "v1alpha1",
// 	}

// 	for _, opt := range opts {
// 		opt(i)
// 	}

// 	return i, nil
// }

func Marshal(i *Island) (string, error) {
	out, err := json.Marshal(i)
	if err != nil {
		return "", err
	}
	return string(out), nil
}

func Unmarshal(island string) (*Island, error) {
	i := &Island{}
	err := json.Unmarshal([]byte(island), i)
	if err != nil {
		return nil, err
	}
	return i, nil
}

// func CreateIsland(world donburi.World, ani *animations.Animations, width, height int, worldX, worldY float64) *donburi.Entry {
// 	// Create the Island entity
// 	islandEntity := world.Create(IslandType)
// 	islandEntry := world.Entry(islandEntity)

// 	// Initialize the Island component
// 	island := &Island{
// 		Width:  width,
// 		Height: height,
// 		X:      worldX,
// 		Y:      worldY,
// 		Tiles:  make([][]*donburi.Entry, height),
// 	}

// 	// Populate the island with tiles
// 	for y := 0; y < height; y++ {
// 		island.Tiles[y] = make([]*donburi.Entry, width)
// 		for x := 0; x < width; x++ {
// 			tileEntity := world.Create(BuildingType, PositionType, TileType, AnimationType)
// 			tileEntry := world.Entry(tileEntity)

// 			// Set components for the tile
// 			PositionType.Set(tileEntry, &Position{X: worldX + float64(x), Y: worldY + float64(y)})
// 			TileType.Set(tileEntry, &Tile{})
// 			anim := ani.GetAnimation(1804, rotation.DEG0)
// 			AnimationType.Set(tileEntry, &Animation{
// 				Count:        anim.Steps,
// 				Duration:     float64(anim.FrameDuration),
// 				Running:      true,
// 				Loop:         true,
// 				CurrentFrame: 0,
// 				CurrentTime:  0,
// 			})
// 			BuildingType.Set(tileEntry, &Building{BuildingID: 1804, Rotation: rotation.DEG0})

// 			// Add the tile entry to the island grid
// 			island.Tiles[y][x] = tileEntry
// 		}
// 	}

// 	// Set the Island component
// 	IslandType.Set(islandEntry, island)

// 	return islandEntry
// }

func CreateIslandFromChunk(world donburi.World, ani *animations.Animations, i *island5.Island5, worldX, worldY float64) *donburi.Entry {
	// Create the Island entity
	islandEntity := world.Create(IslandType)
	islandEntry := world.Entry(islandEntity)

	// Initialize the Island component
	island := &Island{
		Width:   i.Width,
		Height:  i.Height,
		X:       worldX,
		Y:       worldY,
		Climate: Climate(i.Climate),
		Layers:  map[string][]*donburi.Entry{},
	}
	island.Layers[buildings.KindBuildingsID] = []*donburi.Entry{}
	island.Layers[buildings.KindSeaID] = []*donburi.Entry{}
	island.Layers[buildings.KindRoadsID] = []*donburi.Entry{}
	island.Layers[buildings.KindGroundID] = []*donburi.Entry{}

	sorted2DByXY := make(map[int]map[int]island5.Field)
	for _, field := range i.Layers.Top.Fields {
		if _, ok := sorted2DByXY[field.Posy]; !ok {
			sorted2DByXY[field.Posy] = make(map[int]island5.Field)
		}
		sorted2DByXY[field.Posy][field.Posx] = field
	}

	for y := 0; y < i.Height; y++ {
		for x := 0; x < i.Width; x++ {
			if _, ok := sorted2DByXY[y][x]; !ok {
				continue
			}
			field := sorted2DByXY[y][x]

			// for _, field := range i.Layers.Top.Fields {
			// 	x := field.Posx
			// 	y := field.Posy

			currentTile := field
			if currentTile.Id == 65535 {
				fmt.Println("Id 65535")
				continue
			}
			// fmt.Println("currentTile.Id", currentTile.Id)
			tileEntity := world.Create(BuildingType, PositionType, TileType, AnimationType)
			tileEntry := world.Entry(tileEntity)
			posOffset := i.Buildings.Buildings[currentTile.Id].PositionOffset
			size := i.Buildings.Buildings[currentTile.Id].Size
			// Set components for the tile
			PositionType.Set(tileEntry, &Position{X: worldX + float64(x), Y: worldY + float64(y), Offset: float64(posOffset)})
			TileType.Set(tileEntry, &Tile{
				Size: Size{Width: size.W, Height: size.H},
			})
			anim := ani.GetAnimation(currentTile.Id, rotation.DEG0)
			AnimationType.Set(tileEntry, &Animation{
				Count:        anim.Steps,
				Duration:     float64(anim.FrameDuration),
				Running:      true,
				Loop:         true,
				CurrentFrame: 0,
				CurrentTime:  0,
			})
			BuildingType.Set(tileEntry, &Building{
				BuildingID: currentTile.Id,
				Rotation:   rotation.Rotation(currentTile.Orientation),
				Size:       building.BuildingSize(size.W, size.H),
			})

			switch {
			case field.Kind.IsBuilding():
				island.Layers[buildings.KindBuildingsID] = append(island.Layers[buildings.KindBuildingsID], tileEntry)
				// if size.W > 1 || size.H > 2 {
				// 	fmt.Println("church")
				// }
				// for dy := 0; dy < size.H; dy++ {
				// 	for dx := 0; dx < size.W; dx++ {
				// 		occupyEntity := world.Create(BuildingType, PositionType, TileType, AnimationType)
				// 		occupyEntry := world.Entry(occupyEntity)
				// 		BuildingType.Set(occupyEntry, &Building{
				// 			BuildingID: -1,
				// 			Rotation:   0,
				// 			Size:       building.BuildingSize(1, 1),
				// 		})
				// 		AnimationType.Set(occupyEntry, &Animation{
				// 			Count:    1,
				// 			Duration: 1,
				// 		})
				// 		p := &Position{X: worldX + float64(x+dx), Y: worldY + float64(y+dx), Offset: float64(posOffset)}
				// 		PositionType.Set(occupyEntry, p)
				// 		TileType.Set(occupyEntry, &Tile{
				// 			Size:       Size{Width: 1, Height: 1},
				// 			Occupation: true,
				// 		})

				// 		island.Layers[buildings.KindBuildingsID] = append(island.Layers[buildings.KindBuildingsID], occupyEntry)

				// 	}
				// }
			case field.Kind.IsWater():
				island.Layers[buildings.KindSeaID] = append(island.Layers[buildings.KindSeaID], tileEntry)
			case field.Kind.IsRoad():
				island.Layers[buildings.KindRoadsID] = append(island.Layers[buildings.KindRoadsID], tileEntry)
			case field.Kind.IsGround():
				island.Layers[buildings.KindGroundID] = append(island.Layers[buildings.KindGroundID], tileEntry)
			case field.Kind.IsForrest():
				island.Layers[buildings.KindForrestID] = append(island.Layers[buildings.KindForrestID], tileEntry)
			}

		}
	}

	// Set the Island component
	IslandType.Set(islandEntry, island)

	return islandEntry
}
