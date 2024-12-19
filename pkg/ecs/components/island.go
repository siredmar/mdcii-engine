package components

import (
	"encoding/json"
	"fmt"

	"github.com/siredmar/mdcii-engine/pkg/building"
	buildingRotation "github.com/siredmar/mdcii-engine/pkg/building"
	island5 "github.com/siredmar/mdcii-engine/pkg/chunks"
	"github.com/siredmar/mdcii-engine/pkg/cod/buildings"
	buildingsCod "github.com/siredmar/mdcii-engine/pkg/cod/buildings"
	"github.com/siredmar/mdcii-engine/pkg/world/rotation"
	"github.com/siredmar/mdcii-engine/pkg/world/zoom"
	"github.com/yohamta/donburi"

	animations "github.com/siredmar/mdcii-engine/pkg/texture/animations"
)

type Island struct {
	Width, Height int                         // Dimensions of the island
	X, Y          float64                     // Position of the island in the world
	Climate       Climate                     `json:"climate"`
	Tiles         map[string][]*donburi.Entry // 2D grid of tile entries
}

var IslandType = donburi.NewComponentType[Island]()

type Climate int

const (
	North Climate = iota
	South
	Any
)

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

func CreateIslandFromChunk(world donburi.World, cod *buildingsCod.Buildings, ani *animations.Animations, i *island5.Island5, worldX, worldY float64) *donburi.Entry {
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
		Tiles:   map[string][]*donburi.Entry{},
	}
	island.Tiles[buildings.KindBuildingsID] = []*donburi.Entry{}
	island.Tiles[buildings.KindSeaID] = []*donburi.Entry{}
	island.Tiles[buildings.KindRoadsID] = []*donburi.Entry{}
	island.Tiles[buildings.KindGroundID] = []*donburi.Entry{}

	for _, field := range i.Layers.Top.Fields {
		x := field.Posx
		y := field.Posy

		currentTile := field
		if currentTile.Id == 65535 {
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
			Size: Size{Width: size.W, Height: size.H, Z: size.H - zoom.TileHeight()},
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
		building := &Building{
			BuildingID: currentTile.Id,
			Rotation:   rotation.Rotation(currentTile.Orientation),
			Size:       building.BuildingSize(size.W, size.H),
		}
		BuildingType.Set(tileEntry, building)

		switch {
		case field.Kind.IsBuilding():
			island.Tiles[buildings.KindBuildingsID] = append(island.Tiles[buildings.KindBuildingsID], tileEntry)
			if size.W > 1 || size.H > 2 {
				fmt.Println("church")
			}

			fmt.Println("building", currentTile.Id, "x", x, "y", y, "size", size.W, size.H)
			realSize := buildingRotation.BuildingRotationSizes[building.Size][building.Rotation]

			for dy := 0; dy < realSize.Height; dy++ {
				for dx := 0; dx < realSize.Width; dx++ {
					occX := x + dx
					occY := y + dy
					if occX == x && occY == y {
						continue
					}
					fmt.Println("occupy: ", x+dx, y+dy)
					occupyEntity := world.Create(BuildingType, PositionType, TileType, AnimationType)
					occupyEntry := world.Entry(occupyEntity)
					BuildingType.Set(occupyEntry, &Building{
						BuildingID: -1,
						Rotation:   0,
						Size:       buildingRotation.BuildingSize(1, 1),
						// HighFlag:   building.HighFlag,
					})
					AnimationType.Set(occupyEntry, &Animation{
						Count:    1,
						Duration: 1,
					})
					PositionType.Set(occupyEntry, &Position{X: worldX + float64(occX), Y: worldY + float64(occY), Offset: float64(posOffset)})
					buildingIndex, err := cod.GetBuildingIndexById(currentTile.Id)
					if err != nil {
						return nil
					}

					TileType.Set(occupyEntry, &Tile{
						Size:       Size{Width: 1, Height: 1, Z: cod.BuildingsVector[buildingIndex].HighFlag},
						Image:      nil,
						Occupation: true,
					})

					island.Tiles[buildings.KindBuildingsID] = append(island.Tiles[buildings.KindBuildingsID], occupyEntry)

				}
			}
		case field.Kind.IsWater():
			island.Tiles[buildings.KindSeaID] = append(island.Tiles[buildings.KindSeaID], tileEntry)
		case field.Kind.IsRoad():
			island.Tiles[buildings.KindRoadsID] = append(island.Tiles[buildings.KindRoadsID], tileEntry)
		case field.Kind.IsGround():
			island.Tiles[buildings.KindGroundID] = append(island.Tiles[buildings.KindGroundID], tileEntry)
		case field.Kind.IsForrest():
			island.Tiles[buildings.KindForrestID] = append(island.Tiles[buildings.KindForrestID], tileEntry)
		}

	}

	// Set the Island component
	IslandType.Set(islandEntry, island)

	return islandEntry
}
