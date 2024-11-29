package mapping

import (
	"fmt"

	"github.com/siredmar/mdcii-engine/pkg/cod/buildings"
)

type Tiles struct {
	Tile [][]int
}

type Building struct {
	Rotations []Tiles
	Size      buildings.BuildingSize
}

// Generate generates a texture for the given building
// It takes the buildings dimensions and the buildings GFX as a starting point
// The GFX is found in the texture atlas. It maps the single building tiles to create one big texture
// The texture is then used to render the building on the screen
func Generate(b *buildings.Building) (*Building, error) {
	ret := &Building{
		Size: b.Size,
	}
	rotations := 1
	if b.Rotate > 0 {
		rotations = 4
	}
	for i := 0; i < rotations; i++ {
		tiles := make([][]int, b.Size.W)
		for x := 0; x < b.Size.W; x++ {
			tiles[x] = make([]int, b.Size.H)
		}
		for x := 0; x < b.Size.W; x++ {
			for y := 0; y < b.Size.H; y++ {
				tiles[x][y] = mapTile(b.Gfx, b.Size.W, b.Size.H, x, y, i)
			}
		}

		ret.Rotations = append(ret.Rotations, Tiles{Tile: tiles})
	}
	return ret, nil
}

var tileMap = map[int]map[int]map[int]int{
	0: { // Rotation 0
		0: {0: 0, 1: 1},
		1: {0: 2, 1: 3},
	},
	1: { // Rotation 1
		0: {0: 2, 1: 0},
		1: {0: 3, 1: 1},
	},
	2: { // Rotation 2
		0: {0: 2, 1: 3},
		1: {0: 1, 1: 0},
	},
	3: { // Rotation 3
		0: {0: 1, 1: 3},
		1: {0: 0, 1: 2},
	},
}

func mapTile(gfx int, w, h, x, y, rotation int) int {
	if w == 2 && h == 2 {
		if rm, ok := tileMap[rotation]; ok {
			if xm, ok := rm[y]; ok {
				if offset, ok := xm[x]; ok {
					return gfx + w*h*rotation + offset
				}
			}
		}
	}
	return -1
}

func Draw(b *Building, rotation int) {
	for y := 0; y < b.Size.H; y++ {
		for x := 0; x < b.Size.W; x++ {
			fmt.Printf("%d/%d: %d ", x, y, b.Rotations[rotation].Tile[x][y])
		}
		fmt.Printf("\n")
	}
}
