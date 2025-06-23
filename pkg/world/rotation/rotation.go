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
