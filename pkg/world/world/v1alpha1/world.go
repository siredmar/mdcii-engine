package v1alpha1

import (
	"github.com/hajimehoshi/ebiten/v2"
	gamParser "github.com/siredmar/mdcii-engine/pkg/gam"
	"github.com/siredmar/mdcii-engine/pkg/world/camera"
	island "github.com/siredmar/mdcii-engine/pkg/world/island/v1alpha1"
)

type World struct {
	Version string
	Width   int
	Height  int
	Islands []island.Island
	Camera  *camera.Camera
}

func NewWorld(gam *gamParser.GamParser) *World {
	return &World{
		Version: "v1alpha1",
		Camera:  camera.GetCamera(),
	}
}

func (w *World) AddIsland(island island.Island) {
	w.Islands = append(w.Islands, island)
}

func (w *World) GetIslands() []island.Island {
	return w.Islands
}

func (w *World) GetVersion() string {
	return w.Version
}

func (w *World) Render(screen *ebiten.Image) error {
	for _, island := range w.Islands {
		if err := island.Render(w.Camera.Rotation, screen); err != nil {
			return err
		}
	}
	return nil
}
