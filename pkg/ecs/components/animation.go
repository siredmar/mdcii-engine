package components

import (
	"github.com/siredmar/mdcii-engine/pkg/world/rotation"
	"github.com/yohamta/donburi"
)

type Animation struct {
	BuildingID   int
	Rotation     rotation.Rotation
	CurrentFrame int
	Duration     float64
	CurrentTime  float64
	Loop         bool
	Running      bool
	Reset        bool
	Next         bool
}

var AnimationType = donburi.NewComponentType[Animation]()
