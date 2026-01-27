package components

import (
	"github.com/hajimehoshi/ebiten/v2"
	"github.com/yohamta/donburi"
)

type Size struct {
	Width  int
	Height int
	Z      int
}

type Tile struct {
	Image      *ebiten.Image // Current frame image
	PivotX     int
	PivotY     int
	Size       Size
	Occupation bool
	// Parent     *donburi.Entry
}

var (
	TileType = donburi.NewComponentType[Tile]()
)
