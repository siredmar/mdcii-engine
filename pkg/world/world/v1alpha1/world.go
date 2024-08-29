package v1alpha1

import island "github.com/siredmar/mdcii-engine/pkg/world/island/v1alpha1"

type World struct {
	Version string
	Width   int
	Height  int
	Islands []island.Island
}

func NewWorld() *World {
	return &World{
		Version: "v1alpha1",
	}
}
