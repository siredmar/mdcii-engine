package rotation

import (
	"errors"
)

// Rotation represents the rotation in degrees
type Rotation int

const (
	DEG0 Rotation = iota
	DEG90
	DEG180
	DEG270
)

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
func IntToRotation(rotation int) (Rotation, error) {
	switch rotation {
	case int(DEG0):
		return DEG0, nil
	case int(DEG90):
		return DEG90, nil
	case int(DEG180):
		return DEG180, nil
	case int(DEG270):
		return DEG270, nil
	default:
		return DEG0, errors.New("[IntToRotation()] Invalid rotation given.")
	}
}

// RotatePosition rotates a position based on the given rotation
func RotatePosition(mapX, mapY, width, height int, rotation Rotation) (int, int) {
	x := mapX
	y := mapY

	switch rotation {
	case DEG0:
		// no change
	case DEG90:
		x = width - mapY - 1
		y = mapX
	case DEG180:
		x = width - mapX - 1
		y = height - mapY - 1
	case DEG270:
		x = mapY
		y = height - mapX - 1
	}

	return x, y
}

// Add returns the result of adding two rotations
func (r Rotation) Add(other Rotation) (Rotation, error) {
	result := (int(r) + int(other)) % int(len([]Rotation{DEG0, DEG90, DEG180, DEG270}))
	return IntToRotation(result)
}

// Subtract returns the result of subtracting two rotations
func (r Rotation) Subtract(other Rotation) (Rotation, error) {
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
