package building

// GenerateTileOffsets computes tile offsets for a building of given width and height
// at the specified rotation. This replaces the hardcoded RotationOffsets map.
// Tiles are iterated in row-major order (y then x), and each coordinate is
// rotated using the standard 90° clockwise rotation formula.
func GenerateTileOffsets(width, height, rotation int) [][2]int {
	offsets := make([][2]int, 0, width*height)
	for y := 0; y < height; y++ {
		for x := 0; x < width; x++ {
			rx, ry := rotatePosition(x, y, width, height, rotation)
			offsets = append(offsets, [2]int{rx, ry})
		}
	}
	return offsets
}

// rotatePosition rotates a local tile coordinate (x, y) within a building
// of given width and height. Rotation is clockwise: 1 = 90°, 2 = 180°, 3 = 270°.
func rotatePosition(x, y, width, height, rotation int) (int, int) {
	switch rotation % 4 {
	case 0:
		return x, y
	case 1:
		return height - 1 - y, x
	case 2:
		return width - 1 - x, height - 1 - y
	case 3:
		return y, width - 1 - x
	default:
		return x, y
	}
}
