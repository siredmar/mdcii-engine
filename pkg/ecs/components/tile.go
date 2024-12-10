package components

import (
	"github.com/siredmar/mdcii-engine/pkg/world/rotation"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/yohamta/donburi"
)

type Tile struct {
	BuildingID int               // Unique identifier for the building
	Rotation   rotation.Rotation // Rotation state (e.g., 0, 90, 180, 270 degrees)
	Image      *ebiten.Image     // Placeholder for current frame
}

var (
	TileType = donburi.NewComponentType[Tile]()
)
