package components

import (
	animation "github.com/siredmar/mdcii-engine/pkg/texture/animations"
	"github.com/yohamta/donburi"
)

type Animation struct {
	Frames       []animation.Frame
	CurrentFrame int
	Count        int
	Duration     float64
	CurrentTime  float64
	Loop         bool
	Running      bool
	Reset        bool
	Next         bool
}

var AnimationType = donburi.NewComponentType[Animation]()
