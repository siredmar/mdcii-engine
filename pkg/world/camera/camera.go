package camera

type Camera struct {
	X         float64
	Y         float64
	Speed     float64
	Zoom      float64
	ZoomSpeed float64
}

func NewCamera(X, Y, Speed, Zoom, ZoomSpeed float64) *Camera {
	return &Camera{
		X:         X,
		Y:         Y,
		Speed:     Speed,
		Zoom:      Zoom,
		ZoomSpeed: ZoomSpeed,
	}
}

// func (c *Camera) Move(x, y float32) {

// }
