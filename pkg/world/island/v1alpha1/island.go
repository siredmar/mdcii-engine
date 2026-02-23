package v1alpha1

import (
	"encoding/json"
	"fmt"

	"github.com/siredmar/mdcii-engine/pkg/chunks"
	"github.com/siredmar/mdcii-engine/pkg/cod/buildings"
	"github.com/siredmar/mdcii-engine/pkg/texture/sprites"
	"github.com/siredmar/mdcii-engine/pkg/world/rotation"
	"github.com/siredmar/mdcii-engine/pkg/world/tiles"
)

type Climate int

const (
	North Climate = iota
	South
	Any
)

type Coast struct {
	ID       int `json:"id"`
	Rotation int `json:"rotation"`
	X        int `json:"x"`
	Y        int `json:"y"`
}

type Terrain struct {
	ID             int `json:"id"`
	Rotation       int `json:"rotation"`
	X              int `json:"x"`
	Y              int `json:"y"`
	AnimationCount int `json:"animation"`
}

type Building struct {
	ID             int `json:"id"`
	Rotation       int `json:"rotation"`
	X              int `json:"x"`
	Y              int `json:"y"`
	AnimationCount int `json:"animation"`
}

type Island struct {
	Version string  `json:"version"`
	Number  int     `json:"number"`
	Width   int     `json:"width"`
	Height  int     `json:"height"`
	Climate Climate `json:"climate"`
	// Coast  []Coast `json:"coasts"`
	// Terrain   []Terrain  `json:"terrain"`
	Water   []*tiles.TerrainTile `json:"water"`
	Coast   []*tiles.TerrainTile `json:"coast"`
	Terrain []*tiles.TerrainTile `json:"terrain"`
	// Buildings []Building           `json:"buildings"`
	// Figures   []Figures            `json:"figures"`
}

type Option func(*Island)

func WithChunk(gfxSprites *sprites.Sprites, b *buildings.Buildings, c *chunks.Island5) Option {
	return func(*Island) {
		i := &Island{}

		i.Width = c.Width
		i.Height = c.Height
		i.Number = c.IslandNumber
		i.Climate = Climate(c.Climate)
		i.Terrain = make([]*tiles.TerrainTile, i.Width*i.Height)

		for y := range i.Height {
			// fmt.Println("")
			for x := range i.Width {
				t := c.Layers.Top.Fields[y*i.Width+x]
				if t.Id == 65535 {
					//  || t.Id == 102 {
					continue
				}
				if x != t.Posx || y != t.Posy {
					fmt.Printf("Error: x: %d, y: %d, PosX: %d, PosY: %d\n", x, y, t.Posx, t.Posy)
				}
				// fmt.Printf("%d ", rotation.Rotation(t.Orientation))
				// t := g.gam.Islands5[0].Layers.Top.Fields[y*g.gam.Islands5[0].Width+x]
				// // 		if t.Id == 65535 || t.Id == 102 {
				// // 			continue
				// // 		}
				// // 		if t.Id == 1201 {
				// // 			fmt.Println("found 1201")
				// // 		}
				// // 		building := g.buildings.Buildings[t.Id]

				building := b.Buildings[t.Id]
				tile := tiles.NewTerrainTile(rotation.Rotation(t.Orientation), x, y, building, gfxSprites)
				// if building.Kind.IsWater() {
				// 	i.Water = append(i.Water, tile)
				// 	continue
				// }
				// if building.Kind.IsCoast() {
				// 	i.Coast = append(i.Coast, tile)
				// 	continue
				// }

				i.Terrain[x+y*i.Width] = tile
				// i.Terrain = append(i.Terrain, tile)
			}
		}
	}
}

func NewIsland(opts ...Option) (*Island, error) {
	i := &Island{
		Version: "v1alpha1",
	}

	for _, opt := range opts {
		opt(i)
	}

	return i, nil
}

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
