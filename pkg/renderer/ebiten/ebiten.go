package ebitenrenderer

import (
	"github.com/hajimehoshi/ebiten/v2"
	"github.com/siredmar/mdcii-engine/pkg/ecs/systems"
	"github.com/siredmar/mdcii-engine/pkg/ecs/world"
	"github.com/siredmar/mdcii-engine/pkg/renderer"
	"github.com/siredmar/mdcii-engine/pkg/world/rotation"
)

type Screen struct {
	image *ebiten.Image
}

func NewScreen(image *ebiten.Image) Screen {
	return Screen{image: image}
}

func (s Screen) Width() int {
	return s.image.Bounds().Dx()
}

func (s Screen) Height() int {
	return s.image.Bounds().Dy()
}

func (s Screen) Clear() {
	s.image.Clear()
}

func (s Screen) Image() *ebiten.Image {
	return s.image
}

type Renderer struct{}

func New() *Renderer {
	return &Renderer{}
}

func (r *Renderer) Render(w *world.World, screen renderer.Screen, grid bool, currentRotation rotation.Rotation) {
	ebitenScreen, ok := screen.(Screen)
	if !ok {
		return
	}
	systems.RenderSystem(w.World, ebitenScreen.Image(), grid, currentRotation)
}

func (r *Renderer) Close() error {
	return nil
}

func (r *Renderer) SampleSelection(w *world.World) {
	systems.SampleSelectionBuffer(w.World)
}
