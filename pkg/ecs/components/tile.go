package components

import (
	"github.com/hajimehoshi/ebiten/v2"
	"github.com/siredmar/mdcii-engine/pkg/texture/atlas"
	"github.com/yohamta/donburi"
)

type Size struct {
	Width  int
	Height int
	Z      int
}

type Tile struct {
	Image      *ebiten.Image // Placeholder for current frame
	Metadata   *atlas.Metadata
	Offset     float64
	Size       Size
	Occupation bool
	// Parent     *donburi.Entry
}

var (
	TileType = donburi.NewComponentType[Tile]()
)
