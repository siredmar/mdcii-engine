package camera

type Camera struct {
	X         float64
	Y         float64
	Speed     float64
	Zoom      float64
	ZoomSpeed float64
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
		}
	}
	return instance
}

func GetCamera() *Camera {
	return instance
}
