package ecs

import (
	rl "github.com/gen2brain/raylib-go/raylib"
	"github.com/yohamta/donburi"
	"github.com/yohamta/donburi/ecs"
	"github.com/yohamta/donburi/filter"
)

// FrameMeta describes one animation frame.
type FrameMeta struct {
	AtlasImageIdx int
	Src           rl.Rectangle
	Duration      float32
	AnchorOffsetX float32
	AnchorOffsetY float32
}

type TransformData struct {
	GridX, GridY   int
	WorldX, WorldY float32
	LocalRot       uint8
	Dirty          bool
}

type BuildingRefData struct {
	ID         int
	VariantIdx uint8
}

type AnimationData struct {
	Frames   []FrameMeta
	FrameIdx int
	Accum    float32
	Loop     bool
	Paused   bool
}

type RenderData struct {
	AtlasImageIdx int
	Src           rl.Rectangle
	AnchorOffsetX float32
	AnchorOffsetY float32
	SortKey       int64
}

type IslandData struct {
	Width, Height int
	GlobalRot     uint8
}

type CameraData struct {
	X, Y             float64
	Zoom             float32
	Rot              uint8
	OffsetX, OffsetY float32 // screen pixel offsets for centering
}

var (
	Transform   = donburi.NewComponentType[TransformData]()
	BuildingRef = donburi.NewComponentType[BuildingRefData]()
	Animation   = donburi.NewComponentType[AnimationData]()
	Render      = donburi.NewComponentType[RenderData]()
	Island      = donburi.NewComponentType[IslandData]()
	Camera      = donburi.NewComponentType[CameraData]()
)

// Queries
var (
	// Adjust queries: use layer id 0 (first layer) per current donburi signature
	qAnimated   = ecs.NewQuery(0, filter.Contains(Transform, Animation, Render))
	qRenderable = ecs.NewQuery(0, filter.Contains(Transform, Render))
	qIsland     = ecs.NewQuery(0, filter.Contains(Island))
	qCamera     = ecs.NewQuery(0, filter.Contains(Camera))
)

func IslandEntity(w donburi.World) (donburi.Entity, *IslandData) {
	var found *donburi.Entry
	qIsland.Each(w, func(e *donburi.Entry) { found = e })
	if found == nil {
		return 0, nil
	}
	return found.Entity(), Island.Get(found)
}

func CameraEntity(w donburi.World) (donburi.Entity, *CameraData) {
	var found *donburi.Entry
	qCamera.Each(w, func(e *donburi.Entry) { found = e })
	if found == nil {
		return 0, nil
	}
	return found.Entity(), Camera.Get(found)
}

// QRenderableEach applies cb to each renderable entity's Transform.
func QRenderableEach(w donburi.World, cb func(*TransformData)) {
	qRenderable.Each(w, func(e *donburi.Entry) {
		cb(Transform.Get(e))
	})
}
