package animation

import (
	"errors"
	"image"
	"time"

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

	for buildingId, imageSetForRotation := range atlas.ImagesMeta {
		a.Animations[buildingId] = make(map[rotation.Rotation]*Animation)
		for _, rot := range rotation.AllRotations {
			animation := imageSetForRotation.Animations[rot]
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
