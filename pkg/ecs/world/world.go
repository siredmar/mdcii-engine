package world

import (
	"github.com/yohamta/donburi"
)

type World struct {
	World donburi.World
}

func New() *World {
	w := &World{
		World: donburi.NewWorld(),
	}
	return w
}
