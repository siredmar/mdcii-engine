package systems

import (
	"fmt"
	"os"
	"time"

	"github.com/hajimehoshi/ebiten/v2"
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
			if ebiten.IsKeyPressed(ebiten.KeyQ) {
				ctrl.Rotation.Increment()
				fmt.Println("Rotation Incremented:", ctrl.Rotation)
				ctrl.LastKeyPressTime = now
			}
			if ebiten.IsKeyPressed(ebiten.KeyE) {
				ctrl.Rotation.Decrement()
				fmt.Println("Rotation Decremented:", ctrl.Rotation)
				ctrl.LastKeyPressTime = now
			}
			if ebiten.IsKeyPressed(ebiten.KeyG) {
				ctrl.GridVisible = !ctrl.GridVisible
				ctrl.LastKeyPressTime = now
			}
			if ebiten.IsKeyPressed(ebiten.KeyEscape) {
				os.Exit(0)
			}
		}

		// 🖱️ Right mouse drag for scrolling
		mouseX, mouseY := ebiten.CursorPosition()

		if ebiten.IsMouseButtonPressed(ebiten.MouseButtonRight) {
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

		if ebiten.IsKeyPressed(ebiten.KeyW) || ebiten.IsKeyPressed(ebiten.KeyArrowUp) {
			cam.Y -= cameraSpeed
		}
		if ebiten.IsKeyPressed(ebiten.KeyS) || ebiten.IsKeyPressed(ebiten.KeyArrowDown) {
			cam.Y += cameraSpeed
		}
		if ebiten.IsKeyPressed(ebiten.KeyA) || ebiten.IsKeyPressed(ebiten.KeyArrowLeft) {
			cam.X -= cameraSpeed
		}
		if ebiten.IsKeyPressed(ebiten.KeyD) || ebiten.IsKeyPressed(ebiten.KeyArrowRight) {
			cam.X += cameraSpeed
		}
	})
}
