package systems

import (
	"fmt"
	"os"
	"time"

	rl "github.com/gen2brain/raylib-go/raylib"
	"github.com/siredmar/mdcii-engine/pkg/ecs/components"
	"github.com/yohamta/donburi"
	"github.com/yohamta/donburi/filter"
)

var inputQuery = donburi.NewQuery(filter.Contains(components.ControlType))
var cameraQuery = donburi.NewQuery(filter.Contains(components.CameraType))

func InputSystem(world donburi.World) {
	const debounce = time.Millisecond * 250
	const cameraSpeed = 10.0

	// 🔁 Handle control input (rotation, toggles, etc.)
	inputQuery.Each(world, func(entry *donburi.Entry) {
		ctrl := components.ControlType.Get(entry)
		now := time.Now()

		if now.Sub(ctrl.LastKeyPressTime) >= debounce {
			if rl.IsKeyDown(rl.KeyQ) {
				ctrl.Rotation.Increment()
				fmt.Println("Rotation Incremented:", ctrl.Rotation)
				ctrl.LastKeyPressTime = now
			}
			if rl.IsKeyDown(rl.KeyE) {
				ctrl.Rotation.Decrement()
				fmt.Println("Rotation Decremented:", ctrl.Rotation)
				ctrl.LastKeyPressTime = now
			}
			if rl.IsKeyDown(rl.KeyG) {
				ctrl.GridVisible = !ctrl.GridVisible
				ctrl.LastKeyPressTime = now
			}
			if rl.IsKeyDown(rl.KeyEscape) {
				os.Exit(0)
			}
		}

		// 🖱️ Right mouse drag for scrolling
		mouse := rl.GetMousePosition()
		mouseX := int(mouse.X)
		mouseY := int(mouse.Y)

		if rl.IsMouseButtonDown(rl.MouseButtonRight) {
			if !ctrl.Dragging {
				// Start drag
				ctrl.Dragging = true
				ctrl.LastMouseX = mouseX
				ctrl.LastMouseY = mouseY
			} else {
				// Continue drag — calculate delta
				dx := float64(mouseX - ctrl.LastMouseX)
				dy := float64(mouseY - ctrl.LastMouseY)

				cameraQuery.Each(world, func(camEntry *donburi.Entry) {
					cam := components.CameraType.Get(camEntry)
					cam.X -= dx
					cam.Y -= dy
				})

				// Update last position
				ctrl.LastMouseX = mouseX
				ctrl.LastMouseY = mouseY
			}
		} else {
			ctrl.Dragging = false
		}
	})

	// 🎮 Handle camera movement (WASD or arrow keys)
	cameraQuery.Each(world, func(entry *donburi.Entry) {
		cam := components.CameraType.Get(entry)

		if rl.IsKeyDown(rl.KeyW) || rl.IsKeyDown(rl.KeyUp) {
			cam.Y -= cameraSpeed
		}
		if rl.IsKeyDown(rl.KeyS) || rl.IsKeyDown(rl.KeyDown) {
			cam.Y += cameraSpeed
		}
		if rl.IsKeyDown(rl.KeyA) || rl.IsKeyDown(rl.KeyLeft) {
			cam.X -= cameraSpeed
		}
		if rl.IsKeyDown(rl.KeyD) || rl.IsKeyDown(rl.KeyRight) {
			cam.X += cameraSpeed
		}
	})
}
