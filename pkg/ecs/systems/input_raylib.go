//go:build raylib

package systems

import (
	"fmt"
	"os"
	"time"

	rl "github.com/gen2brain/raylib-go/raylib"
	"github.com/siredmar/mdcii-engine/pkg/building"
	"github.com/siredmar/mdcii-engine/pkg/config"
	"github.com/siredmar/mdcii-engine/pkg/ecs/components"
	"github.com/siredmar/mdcii-engine/pkg/world/rotation"
	"github.com/yohamta/donburi"
	"github.com/yohamta/donburi/filter"
)

var inputQueryRaylib = donburi.NewQuery(filter.Contains(components.ControlType))
var cameraQueryRaylib = donburi.NewQuery(filter.Contains(components.CameraType))
var worldQueryRaylib = donburi.NewQuery(filter.Contains(components.WorldType))
var islandQueryRaylib = donburi.NewQuery(filter.Contains(components.IslandType))

func InputSystemRaylib(world donburi.World) {
	const debounce = time.Millisecond * 150
	const cameraSpeed = 0.25
	const tileW = 64.0
	const tileH = 32.0

	inputQueryRaylib.Each(world, func(entry *donburi.Entry) {
		ctrl := components.ControlType.Get(entry)
		now := time.Now()

		if now.Sub(ctrl.LastKeyPressTime) >= debounce {
			if rl.IsKeyDown(rl.KeyQ) {
				rotateCameraRaylib(world, ctrl, rotation.DEG90)
				ctrl.LastKeyPressTime = now
			}
			if rl.IsKeyDown(rl.KeyE) {
				rotateCameraRaylib(world, ctrl, rotation.DEG270)
				ctrl.LastKeyPressTime = now
			}
			if rl.IsKeyDown(rl.KeyG) {
				ctrl.GridVisible = !ctrl.GridVisible
				ctrl.LastKeyPressTime = now
			}
			if rl.IsKeyDown(rl.KeyO) {
				ctrl.OverlayVisible = !ctrl.OverlayVisible
				ctrl.LastKeyPressTime = now
			}
			if rl.IsKeyDown(rl.KeyB) {
				ctrl.SelectionBufferView = !ctrl.SelectionBufferView
				ctrl.LastKeyPressTime = now
			}
			if rl.IsKeyDown(rl.KeyEscape) {
				os.Exit(0)
			}

			if rl.IsKeyDown(rl.KeyOne) {
				ctrl.SelectedSize = building.PreviousBuildingSize(ctrl.SelectedSize)
				ctrl.LastKeyPressTime = now
			}
			if rl.IsKeyDown(rl.KeyTwo) {
				ctrl.SelectedSize = building.NextBuildingSize(ctrl.SelectedSize)
				ctrl.LastKeyPressTime = now
			}
		}

		mouseX := int(rl.GetMouseX())
		mouseY := int(rl.GetMouseY())
		if rl.IsMouseButtonDown(rl.MouseRightButton) {
			if !ctrl.Dragging {
				ctrl.Dragging = true
				ctrl.LastMouseX = mouseX
				ctrl.LastMouseY = mouseY
			} else {
				screenDX := float64(mouseX - ctrl.LastMouseX)
				screenDY := float64(mouseY - ctrl.LastMouseY)
				cameraQueryRaylib.Each(world, func(camEntry *donburi.Entry) {
					cam := components.CameraType.Get(camEntry)
					zoomLevel := cam.Zoom
					if zoomLevel <= 0 {
						zoomLevel = 1.0
					}
					speedMultiplier := 11.0 - 10.0*zoomLevel
					panFactor := config.Instance().MousePanFactor
					tileDX, tileDY := ScreenToTile(screenDX*panFactor*speedMultiplier, screenDY*panFactor*speedMultiplier, int(tileW), int(tileH))
					cam.X -= tileDX
					cam.Y -= tileDY
					cam.Initialized = true
				})
				ctrl.LastMouseX = mouseX
				ctrl.LastMouseY = mouseY
			}
		} else {
			ctrl.Dragging = false
		}
	})

	cameraQueryRaylib.Each(world, func(entry *donburi.Entry) {
		cam := components.CameraType.Get(entry)
		zoomLevel := cam.Zoom
		if zoomLevel <= 0 {
			zoomLevel = 1.0
		}
		speedMultiplier := 11.0 - 10.0*zoomLevel
		adjustedSpeed := cameraSpeed * speedMultiplier

		moved := false
		if rl.IsKeyDown(rl.KeyW) || rl.IsKeyDown(rl.KeyUp) {
			cam.X -= adjustedSpeed
			cam.Y -= adjustedSpeed
			moved = true
		}
		if rl.IsKeyDown(rl.KeyS) || rl.IsKeyDown(rl.KeyDown) {
			cam.X += adjustedSpeed
			cam.Y += adjustedSpeed
			moved = true
		}
		if rl.IsKeyDown(rl.KeyA) || rl.IsKeyDown(rl.KeyLeft) {
			cam.X -= adjustedSpeed
			cam.Y += adjustedSpeed
			moved = true
		}
		if rl.IsKeyDown(rl.KeyD) || rl.IsKeyDown(rl.KeyRight) {
			cam.X += adjustedSpeed
			cam.Y -= adjustedSpeed
			moved = true
		}
		if moved {
			cam.Initialized = true
		}
	})

	scrollY := float64(rl.GetMouseWheelMove())
	if scrollY != 0 {
		mouseX := int(rl.GetMouseX())
		mouseY := int(rl.GetMouseY())
		screenW := float64(rl.GetScreenWidth())
		screenH := float64(rl.GetScreenHeight())
		screenCenterX := screenW / 2
		screenCenterY := screenH / 2

		cameraQueryRaylib.Each(world, func(entry *donburi.Entry) {
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
			cam.Zoom = float64(int(cam.Zoom*10+0.5)) / 10
			newZoom := cam.Zoom

			if oldZoom != newZoom {
				camScreenX := (cam.X - cam.Y) * (tileW / 2)
				camScreenY := (cam.X + cam.Y) * (tileH / 2)

				zoomOffset := 1 - oldZoom
				relX := (float64(mouseX) - screenCenterX*zoomOffset) / oldZoom
				relY := (float64(mouseY) - screenCenterY*zoomOffset) / oldZoom
				worldX := relX + camScreenX
				worldY := relY + camScreenY

				newZoomOffset := 1 - newZoom
				newCamScreenX := worldX - (float64(mouseX)-screenCenterX*newZoomOffset)/newZoom
				newCamScreenY := worldY - (float64(mouseY)-screenCenterY*newZoomOffset)/newZoom

				halfW := tileW / 2
				halfH := tileH / 2
				cam.X = newCamScreenX/halfW/2 + newCamScreenY/halfH/2
				cam.Y = newCamScreenY/halfH/2 - newCamScreenX/halfW/2
			}
			cam.Initialized = true
		})
	}

	clampCameraToWorld(world)
	updateMouseTilePositionRaylib(world, tileW, tileH)
}

func rotateCameraRaylib(world donburi.World, ctrl *components.Control, delta rotation.Rotation) {
	var worldWidth, worldHeight int = 500, 500
	worldQueryRaylib.Each(world, func(wEntry *donburi.Entry) {
		wc := components.WorldType.Get(wEntry)
		if wc != nil {
			worldWidth = wc.Width
			worldHeight = wc.Height
		}
	})
	oldRot := ctrl.Rotation
	newRot := oldRot.Add(delta)
	if delta == rotation.DEG270 {
		newRot = oldRot.Subtract(rotation.DEG90)
	}
	cameraQueryRaylib.Each(world, func(camEntry *donburi.Entry) {
		cam := components.CameraType.Get(camEntry)
		oldX, oldY := cam.X, cam.Y
		ax, ay := rotation.UnrotateWorldPosition(cam.X, cam.Y, worldWidth, worldHeight, oldRot)
		cam.X, cam.Y = rotation.RotateWorldPosition(ax, ay, worldWidth, worldHeight, newRot)
		fmt.Printf("Rotation: %s -> %s\n", oldRot.String(), newRot.String())
		fmt.Printf("  Camera: (%.1f, %.1f) -> (%.1f, %.1f)\n", oldX, oldY, cam.X, cam.Y)
		fmt.Printf("  Anchor (unrotated): (%.1f, %.1f)\n", ax, ay)
	})
	ctrl.Rotation = newRot
}

func updateMouseTilePositionRaylib(world donburi.World, tileW, tileH float64) {
	var cam *components.Camera
	cameraQueryRaylib.Each(world, func(entry *donburi.Entry) {
		cam = components.CameraType.Get(entry)
	})
	if cam == nil {
		return
	}

	var ctrl *components.Control
	inputQueryRaylib.Each(world, func(entry *donburi.Entry) {
		ctrl = components.ControlType.Get(entry)
	})
	if ctrl == nil {
		return
	}

	zoomLevel := cam.Zoom
	if zoomLevel <= 0 {
		zoomLevel = 1.0
	}

	mouseX, mouseY := rl.GetMouseX(), rl.GetMouseY()
	screenW := float64(rl.GetScreenWidth())
	screenH := float64(rl.GetScreenHeight())
	screenCenterX := screenW / 2
	screenCenterY := screenH / 2

	relX := (float64(mouseX) - screenCenterX*(1-zoomLevel)) / zoomLevel
	relY := (float64(mouseY) - screenCenterY*(1-zoomLevel)) / zoomLevel

	camScreenX, camScreenY := TileToScreen(cam.X, cam.Y, int(tileW), int(tileH))
	worldScreenX := relX + camScreenX
	worldScreenY := relY + camScreenY

	tileX, tileY := ScreenToTile(worldScreenX, worldScreenY, int(tileW), int(tileH))
	ctrl.MouseTileX = tileX
	ctrl.MouseTileY = tileY

	ctrl.HoveredIsland = -1
	ctrl.HoveredTileID = -1
	islandIndex := 0
	islandQueryRaylib.Each(world, func(entry *donburi.Entry) {
		island := components.IslandType.Get(entry)
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
