package rotation

// Rotation represents the rotation in degrees
type Rotation int

const (
	DEG0 Rotation = iota
	DEG90
	DEG180
	DEG270
)

var AllRotations = []Rotation{DEG0, DEG90, DEG180, DEG270}

func (r Rotation) String() string {
	switch r {
	case DEG0:
		return "DEG0"
	case DEG90:
		return "DEG90"
	case DEG180:
		return "DEG180"
	case DEG270:
		return "DEG270"
	default:
		return "UNKNOWN"
	}
}

// Increment increments the rotation value
func (r *Rotation) Increment() {
	if *r == DEG270 {
		*r = DEG0
	} else {
		*r++
	}
}

// Decrement decrements the rotation value
func (r *Rotation) Decrement() {
	if *r == DEG0 {
		*r = DEG270
	} else {
		*r--
	}
}

// IntToRotation converts an integer to a Rotation
func IntToRotation(rotation int) Rotation {
	switch rotation {
	case int(DEG0):
		return DEG0
	case int(DEG90):
		return DEG90
	case int(DEG180):
		return DEG180
	case int(DEG270):
		return DEG270
	default:
		return DEG0
	}
}

// RotateOffset rotates a tile-local offset based on rotation
func RotateOffset(x, y, width, height int, rot Rotation) (int, int) {
	switch rot {
	case DEG0:
		return x, y
	case DEG90:
		return height - 1 - y, x
	case DEG180:
		return width - 1 - x, height - 1 - y
	case DEG270:
		return y, width - 1 - x
	default:
		return x, y
	}
}

// RotatePosition rotates a local tile coordinate (x, y) within a building of given width and height
// so that the bottom-left corner remains anchored after rotation.
// Rotation is clockwise: DEG90 means rotating right by 90°.
func RotatePosition(x, y, width, height int, rot Rotation) (int, int) {
	switch rot {
	case DEG0:
		return x, y
	case DEG90:
		// Clockwise 90°: rotate (x, y) → (h - 1 - y, x)
		return height - 1 - y, x
	case DEG180:
		// Clockwise 180°: rotate (x, y) → (w - 1 - x, h - 1 - y)
		return width - 1 - x, height - 1 - y
	case DEG270:
		// Clockwise 270°: rotate (x, y) → (y, w - 1 - x)
		return y, width - 1 - x
	default:
		// Fallback: no rotation
		return x, y
	}
}

// Add returns the result of adding two rotations
func (r Rotation) Add(other Rotation) Rotation {
	result := (int(r) + int(other)) % int(len([]Rotation{DEG0, DEG90, DEG180, DEG270}))
	return IntToRotation(result)
}

// Subtract returns the result of subtracting two rotations
func (r Rotation) Subtract(other Rotation) Rotation {
	result := (int(r) - int(other) + int(len([]Rotation{DEG0, DEG90, DEG180, DEG270}))) % int(len([]Rotation{DEG0, DEG90, DEG180, DEG270}))
	return IntToRotation(result)
}

func CartesianToIso(x, y float64, tileSize int) (float64, float64) {
	rx := (x - y) * float64(tileSize/2)
	ry := (x + y) * float64(tileSize/4)
	return rx, ry
}

func IsoToCartesian(x, y float64, tileSize int) (float64, float64) {
	rx := (x/float64(tileSize/2) + y/float64(tileSize/4)) / 2
	ry := (y/float64(tileSize/4) - (x / float64(tileSize/2))) / 2
	return rx, ry
}

// RotateWorldPosition rotates a world position (x, y) around the world center.
// This is used to rotate island positions when the view rotates.
// worldWidth and worldHeight define the world dimensions for finding the center.
// The rotation is applied as if viewing the world from above and rotating clockwise.
func RotateWorldPosition(x, y float64, worldWidth, worldHeight int, rot Rotation) (float64, float64) {
	// World center
	cx := float64(worldWidth) / 2
	cy := float64(worldHeight) / 2

	// Translate to origin (center)
	dx := x - cx
	dy := y - cy

	var rx, ry float64
	switch rot {
	case DEG0:
		return x, y
	case DEG90:
		// Clockwise 90° in tile space (y increases downward): (x,y) -> (-y, x)
		// Then translate back. For 90° rotation, width and height swap,
		// so the new center is at (height/2, width/2)
		rx = -dy
		ry = dx
		return rx + float64(worldHeight)/2, ry + float64(worldWidth)/2
	case DEG180:
		// Clockwise 180°: (x,y) -> (-x, -y) relative to center
		// Center stays the same
		rx = -dx
		ry = -dy
		return rx + cx, ry + cy
	case DEG270:
		// Clockwise 270° in tile space (y increases downward): (x,y) -> (y, -x)
		// Width and height swap, new center is at (height/2, width/2)
		rx = dy
		ry = -dx
		return rx + float64(worldHeight)/2, ry + float64(worldWidth)/2
	default:
		return x, y
	}
}

// UnrotateWorldPosition maps a position expressed in the rotated world coordinate system
// back into the original (DEG0) world coordinate system.
//
// Important: for DEG90/DEG270 the rotated coordinate system has swapped dimensions.
func UnrotateWorldPosition(x, y float64, worldWidth, worldHeight int, rot Rotation) (float64, float64) {
	switch rot {
	case DEG0:
		return x, y
	case DEG90:
		// Rotated space dims are (worldHeight, worldWidth)
		return RotateWorldPosition(x, y, worldHeight, worldWidth, DEG270)
	case DEG180:
		return RotateWorldPosition(x, y, worldWidth, worldHeight, DEG180)
	case DEG270:
		// Rotated space dims are (worldHeight, worldWidth)
		return RotateWorldPosition(x, y, worldHeight, worldWidth, DEG90)
	default:
		return x, y
	}
}
