package components

import (
	"encoding/json"

	"github.com/siredmar/mdcii-engine/pkg/building"
	island5 "github.com/siredmar/mdcii-engine/pkg/chunks"
	"github.com/siredmar/mdcii-engine/pkg/cod/buildings"
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
		Tiles:   map[string][]*donburi.Entry{},
	}
	island.Tiles[buildings.KindBuildingsID] = []*donburi.Entry{}
	island.Tiles[buildings.KindSeaID] = []*donburi.Entry{}
	island.Tiles[buildings.KindSeaID+"_OVERLAY"] = []*donburi.Entry{}
	island.Tiles[buildings.KindRoadsID] = []*donburi.Entry{}
	island.Tiles[buildings.KindGroundID] = []*donburi.Entry{}
	island.Tiles[buildings.KindGroundID+"_OVERLAY"] = []*donburi.Entry{}

	// Use the already-merged Top layer. Final[0] is base terrain, Final[1] is overlay (buildings).
	var baseLayer *island5.IslandHouse
	if len(i.Layers.Final) >= 1 {
		baseLayer = i.Layers.Final[0] // Base terrain for underlay
	}

	for y := 0; y < i.Height; y++ {
		for x := 0; x < i.Width; x++ {
			// Use the merged Top layer which prioritizes buildings over terrain
			currentTile := i.Layers.Top.Get(x, y)
			if currentTile.Id == 0xFFFF {
				currentTile.Id = 0xFFFF
			}

			// Base layer underlay: draw the underlying terrain when the merged top layer has a different tile.
			if baseLayer != nil {
				base := baseLayer.Get(x, y)
				if base.Id != 0xFFFF && (base.Id != currentTile.Id || base.Orientation != currentTile.Orientation) {
					baseB := i.Buildings.Buildings[base.Id]
					baseOffset := baseB.PositionOffset
					size := baseB.Size

					baseEntity := world.Create(BuildingType, PositionType, TileType, AnimationType)
					baseEntry := world.Entry(baseEntity)
					PositionType.Set(baseEntry, &Position{X: worldX + float64(x), Y: worldY + float64(y), Offset: float64(baseOffset)})
					// For tiles with Rotate=0 in COD, apply runtime sprite rotation
					baseSpriteRot := 0
					if baseB.Rotate == 0 && base.Orientation > 0 {
						baseSpriteRot = base.Orientation
					}
					TileType.Set(baseEntry, &Tile{Size: Size{Width: size.W, Height: size.H, Z: size.H - zoom.TileHeight()}, SpriteRotation: baseSpriteRot})
					anim := ani.GetAnimation(base.Id, rotation.Rotation(base.Orientation))
					AnimationType.Set(baseEntry, &Animation{Count: anim.Steps, Duration: float64(anim.FrameDuration), Running: true, Loop: true})
					BuildingType.Set(baseEntry, &Building{BuildingID: base.Id, Rotation: rotation.Rotation(base.Orientation), Size: building.BuildingSize(size.W, size.H)})

					switch {
					case baseB.Kind.IsWater():
						island.Tiles[buildings.KindSeaID+"_OVERLAY"] = append(island.Tiles[buildings.KindSeaID+"_OVERLAY"], baseEntry)
					case baseB.Kind.IsGround():
						island.Tiles[buildings.KindGroundID+"_OVERLAY"] = append(island.Tiles[buildings.KindGroundID+"_OVERLAY"], baseEntry)
					case baseB.Kind.IsRoad():
						island.Tiles[buildings.KindRoadsID] = append(island.Tiles[buildings.KindRoadsID], baseEntry)
					case baseB.Kind.IsForrest():
						island.Tiles[buildings.KindForrestID] = append(island.Tiles[buildings.KindForrestID], baseEntry)
					case baseB.Kind.IsBuilding():
						island.Tiles[buildings.KindBuildingsID] = append(island.Tiles[buildings.KindBuildingsID], baseEntry)
					}
				}
			}

			// Overlay/top layer.
			if currentTile.Id == 0xFFFF {
				continue
			}

			tileEntity := world.Create(BuildingType, PositionType, TileType, AnimationType)
			tileEntry := world.Entry(tileEntity)
			tileB := i.Buildings.Buildings[currentTile.Id]
			posOffset := tileB.PositionOffset
			size := tileB.Size
			PositionType.Set(tileEntry, &Position{X: worldX + float64(x), Y: worldY + float64(y), Offset: float64(posOffset)})
			// For tiles with Rotate=0 in COD, apply runtime sprite rotation based on orientation
			spriteRot := 0
			if tileB.Rotate == 0 && currentTile.Orientation > 0 {
				spriteRot = currentTile.Orientation
			}
			TileType.Set(tileEntry, &Tile{Size: Size{Width: size.W, Height: size.H, Z: size.H - zoom.TileHeight()}, SpriteRotation: spriteRot})
			anim := ani.GetAnimation(currentTile.Id, rotation.Rotation(currentTile.Orientation))
			AnimationType.Set(tileEntry, &Animation{Count: anim.Steps, Duration: float64(anim.FrameDuration), Running: true, Loop: true})
			BuildingType.Set(tileEntry, &Building{BuildingID: currentTile.Id, Rotation: rotation.Rotation(currentTile.Orientation), Size: building.BuildingSize(size.W, size.H)})

			switch {
			case tileB.Kind.IsBuilding():
				island.Tiles[buildings.KindBuildingsID] = append(island.Tiles[buildings.KindBuildingsID], tileEntry)
				for dy := 0; dy < size.H; dy++ {
					for dx := 0; dx < size.W; dx++ {
						occupyEntity := world.Create(BuildingType, PositionType, TileType, AnimationType)
						occupyEntry := world.Entry(occupyEntity)
						BuildingType.Set(occupyEntry, &Building{BuildingID: -1, Rotation: 0, Size: building.BuildingSize(1, 1)})
						AnimationType.Set(occupyEntry, &Animation{Count: 1, Duration: 1})
						p := &Position{X: worldX + float64(x+dx), Y: worldY + float64(y+dy), Offset: float64(posOffset)}
						PositionType.Set(occupyEntry, p)
						TileType.Set(occupyEntry, &Tile{Size: Size{Width: 1, Height: 1, Z: size.H - zoom.TileHeight()}, Image: nil, Occupation: true})
						island.Tiles[buildings.KindBuildingsID] = append(island.Tiles[buildings.KindBuildingsID], occupyEntry)
					}
				}
			case tileB.Kind.IsWater():
				island.Tiles[buildings.KindSeaID] = append(island.Tiles[buildings.KindSeaID], tileEntry)
			case tileB.Kind.IsRoad():
				island.Tiles[buildings.KindRoadsID] = append(island.Tiles[buildings.KindRoadsID], tileEntry)
			case tileB.Kind.IsGround():
				island.Tiles[buildings.KindGroundID] = append(island.Tiles[buildings.KindGroundID], tileEntry)
			case tileB.Kind.IsForrest():
				island.Tiles[buildings.KindForrestID] = append(island.Tiles[buildings.KindForrestID], tileEntry)
			}
		}
	}

	// Set the Island component
	IslandType.Set(islandEntry, island)

	return islandEntry
}
