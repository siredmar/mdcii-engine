package animation

import (
	"errors"

	rl "github.com/gen2brain/raylib-go/raylib"
	"github.com/siredmar/mdcii-engine/pkg/texture/atlas"
	"github.com/siredmar/mdcii-engine/pkg/world/rotation"
)

// Frame describes a single frame within an animation. It references a
// sub-rectangle inside one of the atlas textures by its PNG index.
type Frame struct {
	PNGIndex int          `json:"pngIndex"`
	Src      rl.Rectangle `json:"src"`
}

type Animations struct {
	Animations map[int]map[rotation.Rotation]*Animation `json:"animations"`
}

type Animation struct {
	Frames        []Frame `json:"frames"`
	Steps         int     `json:"steps"`
	Animated      bool    `json:"animated"`
	FrameDuration int     `json:"frameDuration"`
}

func (a *Animations) GetAnimation(buildingId int, rot rotation.Rotation) *Animation {
	// fmt.Printf("GetAnimation for %d\n", buildingId)
	// for r := range rotation.AllRotations {
	// 	utils.PrettyPrint(a.Animations[buildingId][rotation.Rotation(r)])
	// }
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
			frames := make([]Frame, len(animation.Images))
			for i, img := range animation.Images {
				meta := img.Metadata
				frames[i] = Frame{
					PNGIndex: meta.PNGIndex,
					Src: rl.NewRectangle(
						float32(meta.X),
						float32(meta.Y),
						float32(meta.Width),
						float32(meta.Height),
					),
				}
			}
			a.Animations[buildingId][rot] = &Animation{
				Frames:        frames,
				Steps:         animation.Steps,
				Animated:      animation.Steps > 1,
				FrameDuration: 85,
			}
		}
	}
	return a, nil
}
