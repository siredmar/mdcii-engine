package ecs

import (
	"github.com/siredmar/mdcii-engine/pkg/world/zoom"
	"github.com/yohamta/donburi"
	"github.com/yohamta/donburi/ecs"
)

// isometric projection helpers (1x1 baseline). TileH = TileW/2 for square tiles.
func isoProject(gx, gy int, tileW float32) (float32, float32) {
	hw := tileW / 2
	// vertical compression: tileH := tileW/2 -> half of that for diamond center to row step
	hh := tileW / 4
	x := (float32(gx - gy)) * hw
	y := (float32(gx + gy)) * hh
	return x, y
}

// TransformSystem recalculates world coords & sort keys when dirty or global rotation changed.
func TransformSystem(es *ecs.ECS) { // updated signature
	_, island := IslandEntity(es.World)
	if island == nil {
		return
	}
	_, cam := CameraEntity(es.World)
	if cam == nil { // fallback default
		cam = &CameraData{Zoom: 1}
	}
	resortNeeded := globalRotDirty
	qRenderable.Each(es.World, func(e *donburi.Entry) {
		tr := Transform.Get(e)
		if !tr.Dirty && !globalRotDirty {
			return
		}
		wpx, wpy := isoProject(tr.GridX, tr.GridY, float32(zoom.TileSize()))
		rd := Render.Get(e)
		// camera transform: translate relative to cam.X/Y (grid space) then apply zoom and screen offsets
		dx := (wpx + rd.AnchorOffsetX) - float32(cam.X)
		dy := (wpy + rd.AnchorOffsetY) - float32(cam.Y)
		z := cam.Zoom
		tr.WorldX = cam.OffsetX + dx*z
		tr.WorldY = cam.OffsetY + dy*z
		primary := int64(tr.GridX + tr.GridY)
		secondary := int64(tr.GridY)
		rd.SortKey = (primary << 32) | (secondary & 0xffffffff)
		tr.Dirty = false
		resortNeeded = true
	})
	if resortNeeded {
		markResortNeeded()
	}
	globalRotDirty = false
}
