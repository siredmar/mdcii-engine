package v1alpha1

import (
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

// Render was removed from the v1alpha1 island implementation.
// Keep world compilation clean by omitting render logic in this legacy package.
