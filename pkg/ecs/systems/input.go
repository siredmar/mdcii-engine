package systems

import (
	"fmt"
	"os"
	"time"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/siredmar/mdcii-engine/pkg/building"
	"github.com/siredmar/mdcii-engine/pkg/ecs/components"
	"github.com/yohamta/donburi"
	"github.com/yohamta/donburi/filter"
)

var inputQuery = donburi.NewQuery(filter.Contains(components.ControlType))
var cameraQuery = donburi.NewQuery(filter.Contains(components.CameraType))

func InputSystem(world donburi.World) {
	const debounce = time.Millisecond * 150
	const cameraSpeed = 10.0
	const tileW = 64.0
	const tileH = 32.0

	inputQuery.Each(world, func(entry *donburi.Entry) {
		ctrl := components.ControlType.Get(entry)
		now := time.Now()

		// 🔁 ROTATION / GRID / ESC
		if now.Sub(ctrl.LastKeyPressTime) >= debounce {
			if ebiten.IsKeyPressed(ebiten.KeyQ) {
				ctrl.Rotation.Increment()
				ctrl.LastKeyPressTime = now
				fmt.Println("Rotation Incremented:", ctrl.Rotation)
			}
			if ebiten.IsKeyPressed(ebiten.KeyE) {
				ctrl.Rotation.Decrement()
				ctrl.LastKeyPressTime = now
				fmt.Println("Rotation Decremented:", ctrl.Rotation)
			}
			if ebiten.IsKeyPressed(ebiten.KeyG) {
				ctrl.GridVisible = !ctrl.GridVisible
				ctrl.LastKeyPressTime = now
				fmt.Println("Grid toggled:", ctrl.GridVisible)
			}
			if ebiten.IsKeyPressed(ebiten.KeyO) {
				ctrl.OverlayVisible = !ctrl.OverlayVisible
				ctrl.LastKeyPressTime = now
				fmt.Println("Overlay toggled:", ctrl.OverlayVisible)
			}
			if ebiten.IsKeyPressed(ebiten.KeyEscape) {
				os.Exit(0)
			}

			// // 🔁 ALIGNMENT ADJUSTMENT
			// rotationMap := AlignmentMaps[ctrl.Rotation]
			// offset := rotationMap[ctrl.SelectedSize]

			// if ebiten.IsKeyPressed(ebiten.KeyBracketLeft) { // [
			// 	offset[0] -= 1
			// 	rotationMap[ctrl.SelectedSize] = offset
			// 	ctrl.LastKeyPressTime = now
			// 	fmt.Printf("Adjust X -: %s %s -> [%.0f, %.0f]\n", ctrl.Rotation, ctrl.SelectedSize.String(), offset[0], offset[1])
			// }
			// if ebiten.IsKeyPressed(ebiten.KeyBracketRight) { // ]
			// 	offset[0] += 1
			// 	rotationMap[ctrl.SelectedSize] = offset
			// 	ctrl.LastKeyPressTime = now
			// 	fmt.Printf("Adjust X +: %s %s -> [%.0f, %.0f]\n", ctrl.Rotation, ctrl.SelectedSize.String(), offset[0], offset[1])
			// }
			// if ebiten.IsKeyPressed(ebiten.KeySemicolon) { // ;
			// 	offset[1] -= 1
			// 	rotationMap[ctrl.SelectedSize] = offset
			// 	ctrl.LastKeyPressTime = now
			// 	fmt.Printf("Adjust Y -: %s %s -> [%.0f, %.0f]\n", ctrl.Rotation, ctrl.SelectedSize.String(), offset[0], offset[1])
			// }
			// if ebiten.IsKeyPressed(ebiten.KeyApostrophe) { // '
			// 	offset[1] += 1
			// 	rotationMap[ctrl.SelectedSize] = offset
			// 	ctrl.LastKeyPressTime = now
			// 	fmt.Printf("Adjust Y +: %s %s -> [%.0f, %.0f]\n", ctrl.Rotation, ctrl.SelectedSize.String(), offset[0], offset[1])
			// }

			// 🔁 BUILDING SIZE SELECTION
			if ebiten.IsKeyPressed(ebiten.KeyDigit1) {
				ctrl.SelectedSize = building.PreviousBuildingSize(ctrl.SelectedSize)
				ctrl.LastKeyPressTime = now
				fmt.Println("Selected size:", ctrl.SelectedSize.String())
			}
			if ebiten.IsKeyPressed(ebiten.KeyDigit2) {
				ctrl.SelectedSize = building.NextBuildingSize(ctrl.SelectedSize)
				ctrl.LastKeyPressTime = now
				fmt.Println("Selected size:", ctrl.SelectedSize.String())
			}
		}

		// 🖱️ RIGHT CLICK DRAG SCROLL
		mouseX, mouseY := ebiten.CursorPosition()
		if ebiten.IsMouseButtonPressed(ebiten.MouseButtonRight) {
			if !ctrl.Dragging {
				ctrl.Dragging = true
				ctrl.LastMouseX = mouseX
				ctrl.LastMouseY = mouseY
			} else {
				dx := float64(mouseX - ctrl.LastMouseX)
				dy := float64(mouseY - ctrl.LastMouseY)
				cameraQuery.Each(world, func(camEntry *donburi.Entry) {
					cam := components.CameraType.Get(camEntry)
					cam.X -= dx
					cam.Y -= dy
					fmt.Printf("Camera: pos=(%.1f,%.1f)\n", cam.X, cam.Y)
				})
				ctrl.LastMouseX = mouseX
				ctrl.LastMouseY = mouseY
			}
		} else {
			ctrl.Dragging = false
		}
	})

	// 🎮 KEYBOARD SCROLL (WASD)
	cameraQuery.Each(world, func(entry *donburi.Entry) {
		cam := components.CameraType.Get(entry)
		moved := false
		if ebiten.IsKeyPressed(ebiten.KeyW) || ebiten.IsKeyPressed(ebiten.KeyArrowUp) {
			cam.Y -= cameraSpeed
			moved = true
		}
		if ebiten.IsKeyPressed(ebiten.KeyS) || ebiten.IsKeyPressed(ebiten.KeyArrowDown) {
			cam.Y += cameraSpeed
			moved = true
		}
		if ebiten.IsKeyPressed(ebiten.KeyA) || ebiten.IsKeyPressed(ebiten.KeyArrowLeft) {
			cam.X -= cameraSpeed
			moved = true
		}
		if ebiten.IsKeyPressed(ebiten.KeyD) || ebiten.IsKeyPressed(ebiten.KeyArrowRight) {
			cam.X += cameraSpeed
			moved = true
		}
		if moved {
			fmt.Printf("Camera: pos=(%.1f,%.1f)\n", cam.X, cam.Y)
		}
	})
}
