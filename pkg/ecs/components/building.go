package components

import (
	"github.com/siredmar/mdcii-engine/pkg/building"
	"github.com/siredmar/mdcii-engine/pkg/world/rotation"
	"github.com/yohamta/donburi"
)

type Building struct {
	BuildingID int               // Unique identifier for the building
	Rotation   rotation.Rotation // Rotation state (e.g., 0, 90, 180, 270 degrees)
	Size       building.BuildingSizeIdentifier
}

var (
	BuildingType = donburi.NewComponentType[Building]()
)
