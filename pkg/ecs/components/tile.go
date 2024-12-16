package components

import (
	"github.com/hajimehoshi/ebiten/v2"
	"github.com/yohamta/donburi"
)

type Size struct {
	Width  int
	Height int
}

type Tile struct {
	Image *ebiten.Image // Placeholder for current frame
	Size  Size
}

var (
	TileType = donburi.NewComponentType[Tile]()
)
