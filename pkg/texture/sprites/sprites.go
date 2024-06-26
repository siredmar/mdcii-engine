package sprites

import (
	"image"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/siredmar/mdcii-engine/pkg/texture/atlas"
)

type Sprite struct {
	Width  int
	Height int
	Image  *ebiten.Image
}

type Sprites struct {
	Sprites map[int]Sprite
}

func NewSprites(a *atlas.TextureAtlas) (*Sprites, error) {

	s := &Sprites{
		Sprites: make(map[int]Sprite),
	}

	for i, png := range a.Images {
		img := ebiten.NewImageFromImage(png.SubImage(image.Rectangle{
			Min: image.Point{
				X: 0,
				Y: 0,
			},
			Max: image.Point{
				X: a.ImagesMeta[i].Width,
				Y: 0,
			},
		}))
		s.Sprites[i] = Sprite{
			Width:  a.ImagesMeta[i].Width,
			Height: a.ImagesMeta[i].Height,
			Image:  img,
		}
	}

	return s, nil
}
