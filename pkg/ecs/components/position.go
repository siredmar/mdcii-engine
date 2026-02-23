package components

import (
	"github.com/yohamta/donburi"
)

type Position struct {
	X, Y   float64
	Offset float64
}

var (
	PositionType = donburi.NewComponentType[Position]()
)
