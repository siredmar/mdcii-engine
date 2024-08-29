package zoom

const NrOfZooms = 3 // assuming NR_OF_ZOOMS is 3

var elevations = [NrOfZooms]float32{
	20.0,
	20.0 / 2.0,
	20.0 / 4.0,
}

var tileSizes = [NrOfZooms]int{
	64,
	32,
	16,
}
var tileHeights = [NrOfZooms]int{
	31,
	15,
	7,
}

type Zoom float32

const (
	ZoomGFX Zoom = iota
	ZoomMGFX
	ZoomSGFX
)

var (
	globalZoom Zoom = ZoomGFX
)

func Elevation() float32 {
	return elevations[int(globalZoom)]
}

func TileSize() int {
	return tileSizes[int(globalZoom)]
}

func TileHeight() int {
	return tileHeights[int(globalZoom)]
}

func Get() Zoom {
	return globalZoom
}

func Set(z Zoom) {
	globalZoom = z
}

func ZoomIn() {
	if globalZoom == ZoomGFX {
		globalZoom = ZoomMGFX
	} else if globalZoom == ZoomMGFX {
		globalZoom = ZoomSGFX
	}
}

func ZoomOut() {
	if globalZoom == ZoomSGFX {
		globalZoom = ZoomMGFX
	} else if globalZoom == ZoomMGFX {
		globalZoom = ZoomGFX
	}
}
