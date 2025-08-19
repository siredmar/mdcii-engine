package components

import (
	rl "github.com/gen2brain/raylib-go/raylib"
	"github.com/yohamta/donburi"
)

type Size struct {
	Width  int
	Height int
	Z      int
}

type Tile struct {
	Image      rl.Texture2D // Placeholder for current frame
	Size       Size
	Occupation bool
	// Parent     *donburi.Entry
}

var (
	TileType = donburi.NewComponentType[Tile]()
)
