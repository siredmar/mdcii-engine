package components

import (
	"time"

	"github.com/siredmar/mdcii-engine/pkg/building"
	"github.com/siredmar/mdcii-engine/pkg/world/rotation"
	"github.com/yohamta/donburi"
)

type Control struct {
	Rotation         rotation.Rotation
	GridVisible      bool
	OverlayVisible   bool
	LastKeyPressTime time.Time

	Dragging   bool
	LastMouseX int
	LastMouseY int

	SelectedSize building.BuildingSizeIdentifier

	// HoveredIsland is the index of the island the mouse is currently over, or -1 if none
	HoveredIsland int
	// MouseTileX and MouseTileY are the current mouse position in world tile coordinates
	MouseTileX float64
	MouseTileY float64
}

var ControlType = donburi.NewComponentType[Control]()
