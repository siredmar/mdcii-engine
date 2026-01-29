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
	Image          *ebiten.Image // Current frame image
	PivotX         int
	PivotY         int
	Size           Size
	Occupation     bool
	SpriteRotation int // 0-3: number of 90° clockwise rotations to apply at render time
	// Parent     *donburi.Entry
}

var (
	TileType = donburi.NewComponentType[Tile]()
)
