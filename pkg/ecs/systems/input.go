package systems

import (
	"fmt"
	"os"
	"time"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/siredmar/mdcii-engine/pkg/building"
	"github.com/siredmar/mdcii-engine/pkg/cod/buildings"
	"github.com/siredmar/mdcii-engine/pkg/config"
	"github.com/siredmar/mdcii-engine/pkg/ecs/components"
	"github.com/siredmar/mdcii-engine/pkg/world/rotation"
	"github.com/yohamta/donburi"
	"github.com/yohamta/donburi/filter"
)

var inputQuery = donburi.NewQuery(filter.Contains(components.ControlType))
var cameraQuery = donburi.NewQuery(filter.Contains(components.CameraType))
var worldQuery = donburi.NewQuery(filter.Contains(components.WorldType))
var islandQuery = donburi.NewQuery(filter.Contains(components.IslandType))

func InputSystem(world donburi.World) {
	const debounce = time.Millisecond * 150
	const cameraSpeed = 0.25 // Speed in tile units per frame (WASD)
	const tileW = 64.0
	const tileH = 32.0

	inputQuery.Each(world, func(entry *donburi.Entry) {
		ctrl := components.ControlType.Get(entry)
		now := time.Now()

		// 🔁 ROTATION / GRID / ESC
		if now.Sub(ctrl.LastKeyPressTime) >= debounce {
			if ebiten.IsKeyPressed(ebiten.KeyQ) {
				// Get world dimensions for camera rotation
				var worldWidth, worldHeight int = 500, 500
				worldQuery.Each(world, func(wEntry *donburi.Entry) {
					wc := components.WorldType.Get(wEntry)
					if wc != nil {
						worldWidth = wc.Width
						worldHeight = wc.Height
					}
				})
				// Keep the same world point at screen center by preserving the current
				// camera's world-space anchor across the rotation change.
				oldRot := ctrl.Rotation
				newRot := ctrl.Rotation.Add(rotation.DEG90)
				cameraQuery.Each(world, func(camEntry *donburi.Entry) {
					cam := components.CameraType.Get(camEntry)
					oldX, oldY := cam.X, cam.Y
					ax, ay := rotation.UnrotateWorldPosition(cam.X, cam.Y, worldWidth, worldHeight, oldRot)
					cam.X, cam.Y = rotation.RotateWorldPosition(ax, ay, worldWidth, worldHeight, newRot)
					fmt.Printf("Rotation Q: %s -> %s\n", oldRot.String(), newRot.String())
					fmt.Printf("  Camera: (%.1f, %.1f) -> (%.1f, %.1f)\n", oldX, oldY, cam.X, cam.Y)
					fmt.Printf("  Anchor (unrotated): (%.1f, %.1f)\n", ax, ay)
				})
				// Log island positions
				fmt.Println("  Island positions (original -> rotated):")
				islandQuery.Each(world, func(entry *donburi.Entry) {
					island := components.IslandType.Get(entry)
					rotX, rotY := rotation.RotateWorldPosition(island.X, island.Y, worldWidth, worldHeight, newRot)
					fmt.Printf("    Island at (%.0f, %.0f) -> (%.1f, %.1f)\n", island.X, island.Y, rotX, rotY)
				})
				ctrl.Rotation.Increment()
				ctrl.LastKeyPressTime = now
			}
			if ebiten.IsKeyPressed(ebiten.KeyE) {
				// Get world dimensions for camera rotation
				var worldWidth, worldHeight int = 500, 500
				worldQuery.Each(world, func(wEntry *donburi.Entry) {
					wc := components.WorldType.Get(wEntry)
					if wc != nil {
						worldWidth = wc.Width
						worldHeight = wc.Height
					}
				})
				// Keep the same world point at screen center by preserving the current
				// camera's world-space anchor across the rotation change.
				oldRot := ctrl.Rotation
				newRot := ctrl.Rotation.Subtract(rotation.DEG90)
				cameraQuery.Each(world, func(camEntry *donburi.Entry) {
					cam := components.CameraType.Get(camEntry)
					oldX, oldY := cam.X, cam.Y
					ax, ay := rotation.UnrotateWorldPosition(cam.X, cam.Y, worldWidth, worldHeight, oldRot)
					cam.X, cam.Y = rotation.RotateWorldPosition(ax, ay, worldWidth, worldHeight, newRot)
					fmt.Printf("Rotation E: %s -> %s\n", oldRot.String(), newRot.String())
					fmt.Printf("  Camera: (%.1f, %.1f) -> (%.1f, %.1f)\n", oldX, oldY, cam.X, cam.Y)
					fmt.Printf("  Anchor (unrotated): (%.1f, %.1f)\n", ax, ay)
				})
				// Log island positions
				fmt.Println("  Island positions (original -> rotated):")
				islandQuery.Each(world, func(entry *donburi.Entry) {
					island := components.IslandType.Get(entry)
					rotX, rotY := rotation.RotateWorldPosition(island.X, island.Y, worldWidth, worldHeight, newRot)
					fmt.Printf("    Island at (%.0f, %.0f) -> (%.1f, %.1f)\n", island.X, island.Y, rotX, rotY)
				})
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
			if ebiten.IsKeyPressed(ebiten.KeyB) {
				ctrl.SelectionBufferView = !ctrl.SelectionBufferView
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
				// Pan speed scales linearly with zoom: faster when zoomed out
				cameraQuery.Each(world, func(camEntry *donburi.Entry) {
					cam := components.CameraType.Get(camEntry)
					zoomLevel := cam.Zoom
					if zoomLevel <= 0 {
						zoomLevel = 1.0
					}
					// Speed multiplier: 10x at zoom 0.1, 1x at zoom 1.0
					speedMultiplier := 11.0 - 10.0*zoomLevel
					panFactor := config.Instance().MousePanFactor
					// Use ScreenToTile to convert the delta (treating it as a position from origin)
					tileDX, tileDY := ScreenToTile(screenDX*panFactor*speedMultiplier, screenDY*panFactor*speedMultiplier, int(tileW), int(tileH))
					cam.X -= tileDX
					cam.Y -= tileDY
					cam.Initialized = true // Mark camera as moved by user
				})
				ctrl.LastMouseX = mouseX
				ctrl.LastMouseY = mouseY
			}
		} else {
			ctrl.Dragging = false
		}
	})

	// 🎮 KEYBOARD SCROLL (WASD) - movement in tile coordinates
	// Pan speed scales linearly with zoom: 10x at zoom 0.1, 1x at zoom 1.0
	// Formula: speed = 11 - 10*zoom
	cameraQuery.Each(world, func(entry *donburi.Entry) {
		cam := components.CameraType.Get(entry)
		zoomLevel := cam.Zoom
		if zoomLevel <= 0 {
			zoomLevel = 1.0
		}
		speedMultiplier := 11.0 - 10.0*zoomLevel
		adjustedSpeed := cameraSpeed * speedMultiplier

		moved := false
		if ebiten.IsKeyPressed(ebiten.KeyW) || ebiten.IsKeyPressed(ebiten.KeyArrowUp) {
			// Moving "up" in isometric view means decreasing both X and Y in tile space
			cam.X -= adjustedSpeed
			cam.Y -= adjustedSpeed
			moved = true
		}
		if ebiten.IsKeyPressed(ebiten.KeyS) || ebiten.IsKeyPressed(ebiten.KeyArrowDown) {
			// Moving "down" in isometric view means increasing both X and Y in tile space
			cam.X += adjustedSpeed
			cam.Y += adjustedSpeed
			moved = true
		}
		if ebiten.IsKeyPressed(ebiten.KeyA) || ebiten.IsKeyPressed(ebiten.KeyArrowLeft) {
			// Moving "left" in isometric view means decreasing X, increasing Y in tile space
			cam.X -= adjustedSpeed
			cam.Y += adjustedSpeed
			moved = true
		}
		if ebiten.IsKeyPressed(ebiten.KeyD) || ebiten.IsKeyPressed(ebiten.KeyArrowRight) {
			// Moving "right" in isometric view means increasing X, decreasing Y in tile space
			cam.X += adjustedSpeed
			cam.Y -= adjustedSpeed
			moved = true
		}
		if moved {
			cam.Initialized = true // Mark camera as moved by user
		}
	})

	// 🔍 MOUSE WHEEL ZOOM - zoom levels from 0.1 to 1.0 in 0.1 steps
	// Zoom centers on mouse cursor position
	_, scrollY := ebiten.Wheel()
	if scrollY != 0 {
		mouseX, mouseY := ebiten.CursorPosition()
		screenW, screenH := ebiten.WindowSize()
		screenCenterX := float64(screenW) / 2
		screenCenterY := float64(screenH) / 2

		cameraQuery.Each(world, func(entry *donburi.Entry) {
			cam := components.CameraType.Get(entry)
			const zoomStep = 0.1
			const minZoom = 0.1
			const maxZoom = 1.0

			oldZoom := cam.Zoom
			if scrollY > 0 {
				cam.Zoom += zoomStep
				if cam.Zoom > maxZoom {
					cam.Zoom = maxZoom
				}
			} else {
				cam.Zoom -= zoomStep
				if cam.Zoom < minZoom {
					cam.Zoom = minZoom
				}
			}
			// Round to nearest 0.1 to avoid floating point drift
			cam.Zoom = float64(int(cam.Zoom*10+0.5)) / 10
			newZoom := cam.Zoom

			if oldZoom != newZoom {
				// Camera is in tile coords. Convert to isometric screen coords.
				camScreenX := (cam.X - cam.Y) * (float64(tileW) / 2)
				camScreenY := (cam.X + cam.Y) * (float64(tileH) / 2)

				// Renderer uses: screenPos = relPos * zoom + screenCenter * (1 - zoom)
				// where relPos = worldPos - camScreenPos
				// Inverse: relPos = (screenPos - screenCenter * (1 - zoom)) / zoom
				// worldPos = relPos + camScreenPos
				zoomOffset := 1 - oldZoom
				relX := (float64(mouseX) - screenCenterX*zoomOffset) / oldZoom
				relY := (float64(mouseY) - screenCenterY*zoomOffset) / oldZoom
				worldX := relX + camScreenX
				worldY := relY + camScreenY

				// After zoom, we want the same world point under the cursor
				// screenPos = (worldPos - newCamScreenPos) * newZoom + screenCenter * (1 - newZoom)
				// Solve for newCamScreenPos:
				// (mousePos - screenCenter * (1 - newZoom)) / newZoom = worldPos - newCamScreenPos
				// newCamScreenPos = worldPos - (mousePos - screenCenter * (1 - newZoom)) / newZoom
				newZoomOffset := 1 - newZoom
				newCamScreenX := worldX - (float64(mouseX)-screenCenterX*newZoomOffset)/newZoom
				newCamScreenY := worldY - (float64(mouseY)-screenCenterY*newZoomOffset)/newZoom

				// Convert back to tile coordinates
				// screenX = (tileX - tileY) * (tileW/2)
				// screenY = (tileX + tileY) * (tileH/2)
				// tileX = screenX/(tileW/2)/2 + screenY/(tileH/2)/2
				// tileY = screenY/(tileH/2)/2 - screenX/(tileW/2)/2
				halfW := float64(tileW) / 2
				halfH := float64(tileH) / 2
				cam.X = newCamScreenX/halfW/2 + newCamScreenY/halfH/2
				cam.Y = newCamScreenY/halfH/2 - newCamScreenX/halfW/2
			}
			cam.Initialized = true
		})
	}

	// Clamp camera position to world bounds (now in tile coordinates)
	clampCameraToWorld(world)

	// Update mouse tile position and detect hovered island
	updateMouseTilePosition(world, tileW, tileH)
}

// clampCameraToWorld clamps the camera position to stay within world bounds.
// Camera position is in tile grid coordinates.
func clampCameraToWorld(world donburi.World) {
	var worldComp *components.World
	worldQuery.Each(world, func(entry *donburi.Entry) {
		worldComp = components.WorldType.Get(entry)
	})
	if worldComp == nil || (worldComp.Width == 0 && worldComp.Height == 0) {
		return
	}

	// Camera coordinates are in the *current rotated world-space*.
	// For 90°/270° rotations the effective bounds are swapped.
	rot := rotation.DEG0
	inputQuery.Each(world, func(entry *donburi.Entry) {
		ctrl := components.ControlType.Get(entry)
		if ctrl != nil {
			rot = ctrl.Rotation
		}
	})

	w := float64(worldComp.Width)
	h := float64(worldComp.Height)
	if rot == rotation.DEG90 || rot == rotation.DEG270 {
		w, h = h, w
	}

	cameraQuery.Each(world, func(entry *donburi.Entry) {
		cam := components.CameraType.Get(entry)

		// Clamp X coordinate within world bounds [0, Width)
		if cam.X < 0 {
			cam.X = 0
		}
		if cam.X >= w {
			cam.X = w - 1
		}

		// Clamp Y coordinate within world bounds [0, Height)
		if cam.Y < 0 {
			cam.Y = 0
		}
		if cam.Y >= h {
			cam.Y = h - 1
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

	// Detect which island the mouse is over and look up tile ID
	ctrl.HoveredIsland = -1
	ctrl.HoveredTileID = -1
	islandIndex := 0
	islandQuery.Each(world, func(entry *donburi.Entry) {
		island := components.IslandType.Get(entry)

		// Check if mouse tile is within this island's bounds
		localX := tileX - island.X
		localY := tileY - island.Y

		if localX >= 0 && localX < float64(island.Width) &&
			localY >= 0 && localY < float64(island.Height) {
			ctrl.HoveredIsland = islandIndex
			ctrl.HoveredTileID = lookupTileID(island, tileX, tileY)
		}
		islandIndex++
	})
}

// lookupTileID finds the topmost tile/building ID at the given world tile position.
// It checks layers in top-down order: Buildings > Forest > Roads > Ground > Sea.
func lookupTileID(island *components.Island, worldX, worldY float64) int {
	gridX := int(worldX)
	gridY := int(worldY)

	layerOrder := []string{
		buildings.KindBuildingsID,
		buildings.KindForrestID,
		buildings.KindRoadsID,
		buildings.KindGroundID,
		buildings.KindSeaID,
	}

	for _, kind := range layerOrder {
		entries := island.Tiles[kind]
		for _, entry := range entries {
			if !entry.Valid() {
				continue
			}
			pos := components.PositionType.Get(entry)
			if int(pos.X) == gridX && int(pos.Y) == gridY {
				b := components.BuildingType.Get(entry)
				if b.BuildingID >= 0 {
					return b.BuildingID
				}
			}
		}
	}
	return -1
}
