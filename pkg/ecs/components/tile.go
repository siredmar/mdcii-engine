package components

import (
	rl "github.com/gen2brain/raylib-go/raylib"
	"github.com/yohamta/donburi"
)

type Size struct {
	Width  int
	Height int
}

type Tile struct {
	PNGIndex   int
	Src        rl.Rectangle
	Size       Size
	Occupation bool
	// Parent     *donburi.Entry
}

var (
	TileType = donburi.NewComponentType[Tile]()
)
