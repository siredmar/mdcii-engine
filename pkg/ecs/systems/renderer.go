package systems

import (
	rl "github.com/gen2brain/raylib-go/raylib"
	"github.com/siredmar/mdcii-engine/pkg/cod/buildings"
	"github.com/siredmar/mdcii-engine/pkg/ecs/components"
	r3d "github.com/siredmar/mdcii-engine/pkg/renderer/raylib"
	"github.com/siredmar/mdcii-engine/pkg/world/rotation"
	"github.com/siredmar/mdcii-engine/pkg/world/zoom"
	"github.com/yohamta/donburi"
	"github.com/yohamta/donburi/filter"
)

var rendererQuery = donburi.NewQuery(
	filter.Contains(components.IslandType),
)

// insetRect returns a rectangle inset by the given pixel amount on all sides.
// This helps to avoid sampling artefacts from neighbouring tiles in the atlas.
func insetRect(src rl.Rectangle, px float32) rl.Rectangle {
	return rl.NewRectangle(src.X+px, src.Y+px, src.Width-2*px, src.Height-2*px)
}

// RenderSystem draws all tiles in 3D using raylib.
func RenderSystem(world donburi.World, r *r3d.Renderer, textures []rl.Texture2D) {
	if r == nil {
		return
	}

	var camComp *components.Camera
	camQuery := donburi.NewQuery(filter.Contains(components.CameraType))
	camQuery.Each(world, func(entry *donburi.Entry) {
		camComp = components.CameraType.Get(entry)
	})
	if camComp == nil {
		return
	}

	var rot rotation.Rotation
	controlQuery := donburi.NewQuery(filter.Contains(components.ControlType))
	controlQuery.Each(world, func(entry *donburi.Entry) {
		ctrl := components.ControlType.Get(entry)
		rot = ctrl.Rotation
	})
	globalRotation := rot

	// Update camera based on component
	r.PPU = float32(zoom.TileSize())
	r.OnResize()

	// // Ensure the entire island fits within the view frustum
	// var islandComp *components.Island
	// rendererQuery.Each(world, func(entry *donburi.Entry) {
	// 	islandComp = components.IslandType.Get(entry)
	// })
	// if islandComp != nil {
	// 	maxDim := float32(islandComp.Width)
	// 	if islandComp.Height > islandComp.Width {
	// 		maxDim = float32(islandComp.Height)
	// 	}
	// 	// Add a small margin so edge tiles are fully visible
	// 	r.Camera.Fovy = maxDim + 1
	// }
	currentRotation := globalRotation
	offset := rl.Vector3Subtract(r.Camera.Position, r.Camera.Target)
	r.Camera.Target = rl.NewVector3(float32(camComp.X), 0, float32(camComp.Y))
	r.Camera.Position = rl.Vector3Add(r.Camera.Target, offset)

	r.Begin()

	rendererQuery.Each(world, func(entry *donburi.Entry) {
		island := components.IslandType.Get(entry)

		for _, layerID := range []string{
			buildings.KindSeaID,
			buildings.KindGroundID,
			buildings.KindRoadsID,
			buildings.KindForrestID,
			buildings.KindBuildingsID,
		} {
			for _, tileEntry := range island.Layers[layerID] {
				pos := components.PositionType.Get(tileEntry)
				tile := components.TileType.Get(tileEntry)
				b := components.BuildingType.Get(tileEntry)

				if tile.Src.Width == 0 || tile.Src.Height == 0 {
					continue
				}
				if tile.PNGIndex < 0 || tile.PNGIndex >= len(textures) {
					continue
				}
				tex := textures[tile.PNGIndex]
				if tex.ID == 0 {
					continue
				}

				rotatedX, rotatedY := rotation.RotatePosition(int(pos.X), int(pos.Y), island.Width, island.Height, currentRotation)

				centerX := float32(rotatedX) + float32(b.Size.Width())/2
				centerZ := float32(rotatedY) + float32(b.Size.Height())/2

				// Slightly inset the source rectangle to avoid texture bleeding
				// src := insetRect(tile.Src, 0.5)
				src := tile.Src

				w := src.Width / r.PPU
				h := src.Height / r.PPU

				posY := h/2 + float32(pos.Offset)/(r.PPU)
				position := rl.NewVector3(centerX, posY, centerZ)
				size := rl.NewVector2(w, h)

				// Determine final rotation for the tile and rotate the billboard accordingly
				// Tile orientation is already baked into the texture selection.
				rl.DrawBillboardPro(r.Camera, tex, src, position, rl.NewVector3(0, 1, 0), size, rl.NewVector2(size.X/2, size.Y/2), 0, rl.White)
			}
		}
	})

	r.End()
}
