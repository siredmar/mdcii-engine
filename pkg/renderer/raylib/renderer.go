package raylibrenderer

import (
	"math"

	rl "github.com/gen2brain/raylib-go/raylib"
)

type Renderer struct {
	Camera   rl.Camera3D
	PPU      float32
	AtlasTex rl.Texture2D
}

const (
	isoYawDeg   = 45.0
	isoPitchDeg = 35.264389682 // asin(tan(30°))
)

func (r *Renderer) SetTrueIsoCamera(ppu float32) {
	if ppu > 0 {
		r.PPU = ppu
	}

	// 1 world unit = 1 tile pixel / PPU
	r.Camera.Projection = rl.CameraOrthographic
	r.Camera.Target = rl.NewVector3(0, 0, 0)
	r.Camera.Up = rl.NewVector3(0, 1, 0)

	// Exact yaw/pitch -> camera position on a sphere around the origin.
	yaw := rl.Deg2rad * isoYawDeg
	pitch := rl.Deg2rad * isoPitchDeg
	dist := float32(20) // any positive distance (no perspective in ortho)

	dir := rl.NewVector3(
		float32(math.Cos(pitch)*math.Sin(yaw)),
		float32(math.Sin(pitch)),
		float32(math.Cos(pitch)*math.Cos(yaw)),
	)
	r.Camera.Position = rl.NewVector3(
		-dir.X*dist, -dir.Y*dist, -dir.Z*dist,
	)

	// Ortho height in WORLD units (not degrees):
	r.Camera.Fovy = float32(rl.GetScreenHeight()) / r.PPU
}

func (r *Renderer) OnResize() { // call when window size or zoom changes
	r.Camera.Fovy = float32(rl.GetScreenHeight()) / r.PPU
}

func NewRenderer(ppu float32) *Renderer {
	if ppu <= 0 {
		ppu = 64 // 1 world unit = 64 px
	}
	r := &Renderer{
		PPU: ppu,
	}
	r.SetTrueIsoCamera(ppu)
	// fovy is WORLD HEIGHT, not degrees. Keep world-units == pixels/PPU.
	// r.Camera.Fovy = float32(rl.GetScreenHeight()) / r.PPU
	return r
}

// // Call this if the window size or zoom (PPU) changes.
// func (r *Renderer) UpdateOrthoFovy() {
// 	r.Camera.Fovy = float32(rl.GetScreenHeight()) / r.PPU
// }

func (r *Renderer) SetAtlasTexture(tex rl.Texture2D) {
	r.AtlasTex = tex
	rl.SetTextureFilter(r.AtlasTex, rl.FilterPoint) // pixel-perfect
	rl.SetTextureWrap(r.AtlasTex, rl.WrapClamp)     // no wrap sampling
}
func (r *Renderer) Begin() {
	rl.BeginMode3D(r.Camera)
}
func (r *Renderer) End() {
	rl.EndMode3D()
}

func insetRect(src rl.Rectangle, px float32) rl.Rectangle {
	return rl.NewRectangle(src.X+px, src.Y+px, src.Width-2*px, src.Height-2*px)
}

func (r *Renderer) DrawBillboardFromAtlas(src rl.Rectangle, wPx, hPx float32, tileX, tileZ int) {
	if r.AtlasTex.ID <= 0 {
		return
	}

	// 0.5–1.0 px usually enough; increase if you still see fringes (esp. with mipmaps)
	src = insetRect(src, 0.5)

	worldW := wPx / r.PPU
	worldH := hPx / r.PPU
	pos := rl.NewVector3(float32(tileX)+0.5, worldH*0.5, float32(tileZ)+0.5)
	size := rl.NewVector2(worldW, worldH)
	rl.DrawBillboardRec(r.Camera, r.AtlasTex, src, pos, size, rl.White)
}
