package components

import (
	"time"

	"github.com/siredmar/mdcii-engine/pkg/world/rotation"
	"github.com/yohamta/donburi"
)

type Control struct {
	Rotation         rotation.Rotation
	GridVisible      bool
	LastKeyPressTime time.Time
}

var ControlType = donburi.NewComponentType[Control]()
