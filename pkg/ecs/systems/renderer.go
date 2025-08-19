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

// RenderSystem draws all tiles in 3D using raylib.
func RenderSystem(world donburi.World, r *r3d.Renderer, textures []rl.Texture2D, grid bool, currentRotation rotation.Rotation) {
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

				w := tile.Src.Width / r.PPU
				h := tile.Src.Height / r.PPU

				posY := h/2 - float32(pos.Offset)/r.PPU
				position := rl.NewVector3(centerX, posY, centerZ)
				size := rl.NewVector2(w, h)

				rl.DrawBillboardRec(r.Camera, tex, tile.Src, position, size, rl.White)
			}
		}
	})

	r.End()
}
