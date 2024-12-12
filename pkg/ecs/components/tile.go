package components

import (
	"github.com/hajimehoshi/ebiten/v2"
	"github.com/yohamta/donburi"
)

type Tile struct {
	Image *ebiten.Image // Placeholder for current frame
}

var (
	TileType = donburi.NewComponentType[Tile]()
)
