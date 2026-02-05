//go:build !raylib

package raylibrenderer

import (
	"errors"

	"github.com/siredmar/mdcii-engine/pkg/ecs/world"
	"github.com/siredmar/mdcii-engine/pkg/renderer"
	"github.com/siredmar/mdcii-engine/pkg/world/rotation"
)

type Screen struct {
	width  int
	height int
}

func NewScreen(width, height int) Screen {
	return Screen{width: width, height: height}
}

func (s Screen) Width() int {
	return s.width
}

func (s Screen) Height() int {
	return s.height
}

func (s Screen) Clear() {}

type Renderer struct{}

func New() *Renderer {
	return &Renderer{}
}

func (r *Renderer) Render(_ *world.World, _ renderer.Screen, _ bool, _ rotation.Rotation) {}

func (r *Renderer) Close() error {
	return errors.New("raylib renderer disabled; rebuild with -tags=raylib")
}

func (r *Renderer) TakeScreenshot(_ string) error {
	return errors.New("raylib renderer disabled; rebuild with -tags=raylib")
}

func (r *Renderer) SetAtlasPath(_ string) {}

func (r *Renderer) SampleSelection(_ *world.World) {}
