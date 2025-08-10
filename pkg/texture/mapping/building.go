package mapping

import (
	"github.com/siredmar/mdcii-engine/pkg/building"
	"github.com/siredmar/mdcii-engine/pkg/world/rotation"
)

// GetAnchorOffset returns the tile offset within a building that should align to the world grid
func GetAnchorOffset(size building.BuildingSizeIdentifier, rot rotation.Rotation) (int, int) {
	width := size.Width()
	height := size.Height()

	var anchorX, anchorY int
	switch rot {
	case rotation.DEG0:
		// bottom-left
		anchorX = 0
		anchorY = height - 1
	case rotation.DEG90:
		// bottom-right
		anchorX = width - 1
		anchorY = height - 1
	case rotation.DEG180:
		// top-right
		anchorX = width - 1
		anchorY = 0
	case rotation.DEG270:
		// top-left
		anchorX = 0
		anchorY = 0
	}
	return anchorX, anchorY
}
