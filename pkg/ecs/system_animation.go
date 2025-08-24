package ecs

import (
	"github.com/yohamta/donburi"
	"github.com/yohamta/donburi/ecs"
)

// AnimationSystem advances per-frame animations.
func AnimationSystem(es *ecs.ECS) { // updated signature
	qAnimated.Each(es.World, func(e *donburi.Entry) {
		anim := Animation.Get(e)
		if anim.Paused || len(anim.Frames) == 0 {
			return
		}
		cur := anim.Frames[anim.FrameIdx]
		anim.Accum += deltaTime
		if anim.Accum < cur.Duration {
			return
		}
		anim.Accum -= cur.Duration
		next := anim.FrameIdx + 1
		if next >= len(anim.Frames) {
			if anim.Loop {
				next = 0
			} else {
				next = len(anim.Frames) - 1
				anim.Paused = true
			}
		}
		if next != anim.FrameIdx {
			anim.FrameIdx = next
			frame := anim.Frames[anim.FrameIdx]
			r := Render.Get(e)
			r.AtlasImageIdx = frame.AtlasImageIdx
			r.Src = frame.Src
			r.AnchorOffsetX = frame.AnchorOffsetX
			r.AnchorOffsetY = frame.AnchorOffsetY
			Transform.Get(e).Dirty = true
		}
	})
}
