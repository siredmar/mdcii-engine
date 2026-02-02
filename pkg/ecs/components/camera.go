package components

import (
	"github.com/siredmar/mdcii-engine/pkg/world/rotation"
	"github.com/yohamta/donburi"
)

type Camera struct {
	X           float64
	Y           float64
	Zoom        float64
	Rotation    rotation.Rotation // Custom type from your engine
	Initialized bool              // True if camera was moved by user (should be saved)
}

var CameraType = donburi.NewComponentType[Camera]()
