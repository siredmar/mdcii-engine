package raylibrenderer

import rl "github.com/gen2brain/raylib-go/raylib"

type Renderer struct {
	Camera   rl.Camera3D
	PPU      float32      // pixels per world unit (64 matches your 64x32 base width)
	AtlasTex rl.Texture2D // your atlas texture; set via SetAtlasTexture
}

func NewRenderer(ppu float32) *Renderer {
	if ppu <= 0 {
		ppu = 64
	}
	return &Renderer{
		Camera: rl.Camera3D{
			Position:   rl.NewVector3(10, 10, 10),
			Target:     rl.NewVector3(0, 0, 0),
			Up:         rl.NewVector3(0, 1, 0),
			Fovy:       45,
			Projection: rl.CameraOrthographic,
		},
		PPU: ppu,
	}
}

func (r *Renderer) SetAtlasTexture(tex rl.Texture2D) {
	r.AtlasTex = tex
}

func (r *Renderer) Begin() { rl.BeginMode3D(r.Camera) }
func (r *Renderer) End()   { rl.EndMode3D() }

func (r *Renderer) DrawGroundTile(tileX, tileZ int) {
	x := float32(tileX) + 0.5
	z := float32(tileZ) + 0.5
	rl.DrawPlane(rl.NewVector3(x, 0, z), rl.NewVector2(1, 1), rl.DarkGray)
}

// Draw a billboard using a sub-rectangle from the atlas (pixels).
// Bottom-center sits on (tileX+0.5, 0, tileZ+0.5).
func (r *Renderer) DrawBillboardFromAtlas(src rl.Rectangle, wPx, hPx float32, tileX, tileZ int) {
	if r.AtlasTex.ID <= 0 {
		return
	}

	// Convert pixel size -> world units
	worldW := wPx / r.PPU
	worldH := hPx / r.PPU

	pos := rl.NewVector3(float32(tileX)+0.5, 0, float32(tileZ)+0.5)
	// DrawBillboardRec centers the quad on pos; lift by half height to make bottom touch ground.
	pos.Y += worldH * 0.5

	size := rl.NewVector2(worldW, worldH)
	rl.DrawBillboardRec(r.Camera, r.AtlasTex, src, pos, size, rl.White)
}
