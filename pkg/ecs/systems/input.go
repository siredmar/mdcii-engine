package systems

import (
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
var worldQuery = donburi.NewQuery(filter.Contains(components.WorldType))
var islandQuery = donburi.NewQuery(filter.Contains(components.IslandType))

func InputSystem(world donburi.World) {
	const debounce = time.Millisecond * 150
	const cameraSpeed = 0.5 // Speed in tile units per frame
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
			}
			if ebiten.IsKeyPressed(ebiten.KeyE) {
				ctrl.Rotation.Decrement()
				ctrl.LastKeyPressTime = now
			}
			if ebiten.IsKeyPressed(ebiten.KeyG) {
				ctrl.GridVisible = !ctrl.GridVisible
				ctrl.LastKeyPressTime = now
			}
			if ebiten.IsKeyPressed(ebiten.KeyO) {
				ctrl.OverlayVisible = !ctrl.OverlayVisible
				ctrl.LastKeyPressTime = now
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
			}
			if ebiten.IsKeyPressed(ebiten.KeyDigit2) {
				ctrl.SelectedSize = building.NextBuildingSize(ctrl.SelectedSize)
				ctrl.LastKeyPressTime = now
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
				// Convert screen pixel delta to tile coordinate delta
				screenDX := float64(mouseX - ctrl.LastMouseX)
				screenDY := float64(mouseY - ctrl.LastMouseY)
				// Use ScreenToTile to convert the delta (treating it as a position from origin)
				tileDX, tileDY := ScreenToTile(screenDX, screenDY, int(tileW), int(tileH))
				cameraQuery.Each(world, func(camEntry *donburi.Entry) {
					cam := components.CameraType.Get(camEntry)
					cam.X -= tileDX
					cam.Y -= tileDY
				})
				ctrl.LastMouseX = mouseX
				ctrl.LastMouseY = mouseY
			}
		} else {
			ctrl.Dragging = false
		}
	})

	// 🎮 KEYBOARD SCROLL (WASD) - movement in tile coordinates
	cameraQuery.Each(world, func(entry *donburi.Entry) {
		cam := components.CameraType.Get(entry)
		if ebiten.IsKeyPressed(ebiten.KeyW) || ebiten.IsKeyPressed(ebiten.KeyArrowUp) {
			// Moving "up" in isometric view means decreasing both X and Y in tile space
			cam.X -= cameraSpeed
			cam.Y -= cameraSpeed
		}
		if ebiten.IsKeyPressed(ebiten.KeyS) || ebiten.IsKeyPressed(ebiten.KeyArrowDown) {
			// Moving "down" in isometric view means increasing both X and Y in tile space
			cam.X += cameraSpeed
			cam.Y += cameraSpeed
		}
		if ebiten.IsKeyPressed(ebiten.KeyA) || ebiten.IsKeyPressed(ebiten.KeyArrowLeft) {
			// Moving "left" in isometric view means decreasing X, increasing Y in tile space
			cam.X -= cameraSpeed
			cam.Y += cameraSpeed
		}
		if ebiten.IsKeyPressed(ebiten.KeyD) || ebiten.IsKeyPressed(ebiten.KeyArrowRight) {
			// Moving "right" in isometric view means increasing X, decreasing Y in tile space
			cam.X += cameraSpeed
			cam.Y -= cameraSpeed
		}
	})

	// 🔍 MOUSE WHEEL ZOOM - zoom levels from 0.1 to 1.0 in 0.1 steps
	_, scrollY := ebiten.Wheel()
	if scrollY != 0 {
		cameraQuery.Each(world, func(entry *donburi.Entry) {
			cam := components.CameraType.Get(entry)
			const zoomStep = 0.1
			const minZoom = 0.1
			const maxZoom = 1.0

			if scrollY > 0 {
				// Scroll up = zoom in (increase zoom)
				cam.Zoom += zoomStep
				if cam.Zoom > maxZoom {
					cam.Zoom = maxZoom
				}
			} else {
				// Scroll down = zoom out (decrease zoom)
				cam.Zoom -= zoomStep
				if cam.Zoom < minZoom {
					cam.Zoom = minZoom
				}
			}
			// Round to nearest 0.1 to avoid floating point drift
			cam.Zoom = float64(int(cam.Zoom*10+0.5)) / 10
		})
	}

	// Apply world wrap-around to camera position (now in tile coordinates)
	wrapCameraToWorld(world)

	// Update mouse tile position and detect hovered island
	updateMouseTilePosition(world, tileW, tileH)
}

// wrapCameraToWorld wraps the camera position when it leaves the world bounds.
// Camera position is in tile grid coordinates, so wrapping is straightforward.
func wrapCameraToWorld(world donburi.World) {
	var worldComp *components.World
	worldQuery.Each(world, func(entry *donburi.Entry) {
		worldComp = components.WorldType.Get(entry)
	})
	if worldComp == nil || (worldComp.Width == 0 && worldComp.Height == 0) {
		return
	}

	w := float64(worldComp.Width)
	h := float64(worldComp.Height)

	cameraQuery.Each(world, func(entry *donburi.Entry) {
		cam := components.CameraType.Get(entry)

		// Wrap X coordinate within world bounds [0, Width)
		for cam.X < 0 {
			cam.X += w
		}
		for cam.X >= w {
			cam.X -= w
		}

		// Wrap Y coordinate within world bounds [0, Height)
		for cam.Y < 0 {
			cam.Y += h
		}
		for cam.Y >= h {
			cam.Y -= h
		}
	})
}

// updateMouseTilePosition converts the mouse screen position to world tile coordinates
// and detects which island (if any) the mouse is hovering over.
func updateMouseTilePosition(world donburi.World, tileW, tileH float64) {
	var cam *components.Camera
	cameraQuery.Each(world, func(entry *donburi.Entry) {
		cam = components.CameraType.Get(entry)
	})
	if cam == nil {
		return
	}

	var ctrl *components.Control
	inputQuery.Each(world, func(entry *donburi.Entry) {
		ctrl = components.ControlType.Get(entry)
	})
	if ctrl == nil {
		return
	}

	// Get zoom level (default to 1.0 if not set)
	zoomLevel := cam.Zoom
	if zoomLevel <= 0 {
		zoomLevel = 1.0
	}

	// Get mouse screen position
	mouseX, mouseY := ebiten.CursorPosition()

	// Get screen dimensions for zoom calculations
	// We need to reverse the zoom transformation applied in the renderer
	// The renderer applies: screenPos = relPos * zoom + screenCenter * (1 - zoom)
	// So: relPos = (screenPos - screenCenter * (1 - zoom)) / zoom
	// And: worldScreenPos = relPos + cameraScreenPos

	// Assuming a standard screen size (this should ideally come from the actual screen)
	const screenW, screenH = 1024.0, 1024.0
	screenCenterX := screenW / 2
	screenCenterY := screenH / 2

	// Reverse the zoom transformation to get the world screen position
	relX := (float64(mouseX) - screenCenterX*(1-zoomLevel)) / zoomLevel
	relY := (float64(mouseY) - screenCenterY*(1-zoomLevel)) / zoomLevel

	// Convert camera tile position to screen coordinates
	camScreenX, camScreenY := TileToScreen(cam.X, cam.Y, int(tileW), int(tileH))

	// Mouse world screen position = relative position + camera screen offset
	worldScreenX := relX + camScreenX
	worldScreenY := relY + camScreenY

	// Convert world screen position to tile coordinates
	tileX, tileY := ScreenToTile(worldScreenX, worldScreenY, int(tileW), int(tileH))

	// Store mouse tile position
	ctrl.MouseTileX = tileX
	ctrl.MouseTileY = tileY

	// Detect which island the mouse is over
	ctrl.HoveredIsland = -1
	islandIndex := 0
	islandQuery.Each(world, func(entry *donburi.Entry) {
		island := components.IslandType.Get(entry)

		// Check if mouse tile is within this island's bounds
		localX := tileX - island.X
		localY := tileY - island.Y

		if localX >= 0 && localX < float64(island.Width) &&
			localY >= 0 && localY < float64(island.Height) {
			ctrl.HoveredIsland = islandIndex
		}
		islandIndex++
	})
}
