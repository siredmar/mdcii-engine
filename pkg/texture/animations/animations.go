package animation

import (
	"errors"

	rl "github.com/gen2brain/raylib-go/raylib"
	"github.com/siredmar/mdcii-engine/pkg/texture/atlas"
	"github.com/siredmar/mdcii-engine/pkg/world/rotation"
)

type Animations struct {
	Animations map[int]map[rotation.Rotation]*Animation
}

type Animation struct {
	Frames        []rl.Texture2D
	Steps         int
	Animated      bool
	FrameDuration int
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
				Frames: func() []rl.Texture2D {
					out := []rl.Texture2D{}
					for _, img := range animation.Images {
						rlImg := rl.NewImageFromImage(img.Sprite)
						tex := rl.LoadTextureFromImage(rlImg)
						rl.UnloadImage(rlImg)
						out = append(out, tex)
					}
					return out
				}(),
				Steps:         animation.Steps,
				Animated:      func() bool { return animation.Steps > 1 }(),
				FrameDuration: 85,
			}
		}
	}
	return a, nil
}
