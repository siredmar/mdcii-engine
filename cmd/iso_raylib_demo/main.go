package main

import (
	rl "github.com/gen2brain/raylib-go/raylib"
	r3d "github.com/siredmar/mdcii-engine/pkg/renderer/raylib"
)

func main() {
	rl.InitWindow(1280, 720, "iso raylib demo")
	defer rl.CloseWindow()
	rl.SetTargetFPS(60)

	// Create renderer (64 px = 1 world unit)
	r := r3d.NewRenderer(64)

	// If you have an atlas PNG handy, load it; otherwise you can skip billboard draw.
	// tex := rl.LoadTexture("assets/atlas.png")
	// defer rl.UnloadTexture(tex)
	// r.SetAtlasTexture(tex)

	for !rl.WindowShouldClose() {
		rl.BeginDrawing()
		rl.ClearBackground(rl.Black)

		r.Begin()
		// Draw a 5x5 ground
		for z := 0; z < 5; z++ {
			for x := 0; x < 5; x++ {
				r.DrawGroundTile(x, z)
			}
		}

		// Example billboard (only if you loaded an atlas above):
		// src := rl.NewRectangle(0, 0, 64, 128) // x,y,w,h in pixels within your atlas
		// r.DrawBillboardFromAtlas(src, 64, 128, 2, 2)

		r.End()

		rl.DrawText("WASD/right-drag to move camera (implement as needed)", 10, 10, 20, rl.RayWhite)
		rl.EndDrawing()
	}
}
