package camera

import (
	"image/color"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/hajimehoshi/ebiten/v2/vector"
	rotation "github.com/siredmar/mdcii-engine/pkg/world/rotation"
)

// DrawGrid draws an isometric grid on the dst image.
func DrawGrid(dst *ebiten.Image, width, height int, distX, distY int) {
	var c color.Color = color.RGBA{255, 0, 0, 255} // Red color for grid lines

	// Draw lines parallel to the X-axis (going from left to right in the isometric grid)
	for y := 0; y <= height; y += distY {
		startX, startY := rotation.CartesianToIso(0, float64(y), distX)
		endX, endY := rotation.CartesianToIso(float64(width), float64(y), distX)

		vector.StrokeLine(dst, float32(startX), float32(startY), float32(endX), float32(endY), 1, c, false)
	}

	// Draw lines parallel to the Y-axis (going from top to bottom in the isometric grid)
	for x := 0; x <= width; x += distX {
		startX, startY := rotation.CartesianToIso(float64(x), 0, distX)
		endX, endY := rotation.CartesianToIso(float64(x), float64(height), distX)

		vector.StrokeLine(dst, float32(startX), float32(startY), float32(endX), float32(endY), 1, c, false)
	}
}
