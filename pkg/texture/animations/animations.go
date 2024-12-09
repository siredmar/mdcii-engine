package animation

import (
	"errors"
	"image"
	"time"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/siredmar/mdcii-engine/pkg/texture/atlas"
	"github.com/siredmar/mdcii-engine/pkg/world/rotation"
)

type Animations struct {
	Animations map[int]map[rotation.Rotation]*Animation
}

type Animation struct {
	Frames        []*image.Image
	Steps         int
	Animated      bool
	FrameDuration time.Duration
}

func convert(in *atlas.Animation) []*ebiten.Image {
	out := []*ebiten.Image{}
	//make([]*ebiten.Image, in.Steps)
	for _, img := range in.Images {
		out = append(out, ebiten.NewImageFromImage(img.Sprite))
	}
	return out
}

func (a *Animations) GetAnimation(buildingId int, rot rotation.Rotation) *Animation {
	return a.Animations[buildingId][rot]
}

func New(atlas *atlas.TextureAtlas) (*Animations, error) {
	a := &Animations{
		Animations: make(map[int]map[rotation.Rotation]*Animation),
	}
	if atlas == nil {
		return nil, errors.New("atlas is nil")
	}
	if a.Animations == nil {
		a.Animations = make(map[int]map[rotation.Rotation]*Animation)
	}

	// bid, err := atlas.BuildingsCOD.GetBuildingIdByIndex(381)
	// if err != nil {
	// 	log.Fatal(err)
	// }
	for buildingId, imageSetForRotation := range atlas.ImagesMeta {
		a.Animations[buildingId] = make(map[rotation.Rotation]*Animation)
		for _, rot := range []rotation.Rotation{rotation.DEG0, rotation.DEG180, rotation.DEG270} {
			animation := imageSetForRotation.Animations[rot]
			// c := convert(animation)
			// if buildingId == bid {
			// 	fmt.Println("BuildingIndex 381")
			// 	err := atlas.ExportPNG("out.png", ebitenImageToRGBA(c[0]))
			// 	if err != nil {
			// 		log.Fatal(err)
			// 	}
			// fmt.Println("Animation steps", animation.Steps)
			// fmt.Println("Images", len(c))
			// }
			a.Animations[buildingId][rot] = &Animation{
				Frames: func() []*image.Image {
					out := []*image.Image{}
					for _, img := range animation.Images {
						out = append(out, &img.Sprite)
					}
					return out
				}(),
				Steps:         animation.Steps,
				Animated:      func() bool { return animation.Steps > 1 }(),
				FrameDuration: time.Millisecond * 200,
			}
		}
	}
	return a, nil
}

func ebitenImageToRGBA(ebImg *ebiten.Image) *image.RGBA {
	// Get the size of the ebiten image
	width, height := ebImg.Bounds().Size().X, ebImg.Bounds().Size().Y

	// Create a byte slice to hold the pixel data
	pixels := make([]byte, 4*width*height)

	// Copy pixel data from the ebiten.Image
	ebImg.ReadPixels(pixels)

	// Create an image.RGBA and populate it with pixel data
	rgba := image.NewRGBA(image.Rect(0, 0, width, height))
	copy(rgba.Pix, pixels)

	return rgba
}
