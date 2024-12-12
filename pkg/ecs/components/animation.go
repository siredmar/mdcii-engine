package components

import (
	"github.com/yohamta/donburi"
)

type Animation struct {
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
