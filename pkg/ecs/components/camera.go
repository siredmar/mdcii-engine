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

	// CenterOnIsland triggers centering on first render (when screen size is known)
	// Set to island index + 1 (0 means disabled)
	CenterOnIsland int
}

var CameraType = donburi.NewComponentType[Camera]()
