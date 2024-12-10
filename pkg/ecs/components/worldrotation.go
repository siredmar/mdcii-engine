package components

import (
	"github.com/siredmar/mdcii-engine/pkg/world/rotation"
	"github.com/yohamta/donburi"
)

type WorldRotation struct {
	Rotation rotation.Rotation
}

var (
	WorldRotationType = donburi.NewComponentType[WorldRotation]()
)
