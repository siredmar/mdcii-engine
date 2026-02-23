package mapping

import (
	"github.com/siredmar/mdcii-engine/pkg/building"
	"github.com/siredmar/mdcii-engine/pkg/world/rotation"
)

// GetAnchorOffset returns the tile offset within a building that should align to the world grid
func GetAnchorOffset(size building.BuildingSizeIdentifier, rot rotation.Rotation) (int, int) {
	width := size.Width()
	height := size.Height()

	// Anchor is always the bottom-left tile in DEG0
	anchorX := 0
	anchorY := height - 1

	// Rotate anchor tile position based on current rotation
	rotX, rotY := rotation.RotatePosition(anchorX, anchorY, width, height, rot)
	return rotX, rotY
}
