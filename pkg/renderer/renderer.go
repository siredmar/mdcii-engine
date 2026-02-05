package renderer

import (
	"github.com/siredmar/mdcii-engine/pkg/ecs/world"
	"github.com/siredmar/mdcii-engine/pkg/world/rotation"
)

type Screen interface {
	Width() int
	Height() int
	Clear()
}

type Renderer interface {
	Render(w *world.World, screen Screen, grid bool, currentRotation rotation.Rotation)
	Close() error
	SampleSelection(w *world.World)
}

type NoopRenderer struct{}

func NewNoop() *NoopRenderer {
	return &NoopRenderer{}
}

func (r *NoopRenderer) Render(_ *world.World, screen Screen, _ bool, _ rotation.Rotation) {
	screen.Clear()
}

func (r *NoopRenderer) Close() error {
	return nil
}

func (r *NoopRenderer) SampleSelection(_ *world.World) {}
