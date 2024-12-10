package sprites

import (
	"image"
	"path/filepath"
	"strconv"
	"strings"

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

	for atlasImageIndex, png := range a.Images {
		for i, sub := range a.ImagesMeta[atlasImageIndex] {
			img := ebiten.NewImageFromImage(png.SubImage(image.Rectangle{
				Min: image.Point{
					X: sub.X,
					Y: sub.Y,
				},
				Max: image.Point{
					X: sub.X + sub.Width,
					Y: sub.Y + sub.Height,
				},
			}))
			file := strings.TrimSuffix(filepath.Base(i), filepath.Ext(i))
			index, err := strconv.Atoi(file)
			if err != nil {
				return nil, err
			}

			s.Sprites[index] = Sprite{
				Width:  sub.Width,
				Height: sub.Height,
				Image:  img,
			}
		}
	}

	return s, nil
}
