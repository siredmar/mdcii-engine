package camera

import "github.com/siredmar/mdcii-engine/pkg/world/rotation"

type Camera struct {
	X         float64
	Y         float64
	Speed     float64
	Zoom      float64
	ZoomSpeed float64
	Rotation  rotation.Rotation
}

var (
	instance *Camera
)

func NewCamera(X, Y, Speed, Zoom, ZoomSpeed float64) *Camera {
	if instance == nil {
		instance = &Camera{
			X:         X,
			Y:         Y,
			Speed:     Speed,
			Zoom:      Zoom,
			ZoomSpeed: ZoomSpeed,
			Rotation:  rotation.DEG0,
		}
	}
	return instance
}

func GetCamera() *Camera {
	return instance
}

func (c *Camera) RotateRight() {
	c.Rotation.Increment()
}

func (c *Camera) RotateLeft() {
	c.Rotation.Decrement()
}

func (c *Camera) CurrentRotation() rotation.Rotation {
	return c.Rotation
}
