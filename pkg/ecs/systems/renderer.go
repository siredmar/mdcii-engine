package systems

import (
	rl "github.com/gen2brain/raylib-go/raylib"
	"github.com/siredmar/mdcii-engine/pkg/building"
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
func RenderSystem(world donburi.World, r *r3d.Renderer, grid bool, currentRotation rotation.Rotation) {
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
			for _, tileEntry := range island.Tiles[layerID] {
				pos := components.PositionType.Get(tileEntry)
				tile := components.TileType.Get(tileEntry)
				b := components.BuildingType.Get(tileEntry)

				if tile.Image.ID == 0 {
					continue
				}

				rotatedX, rotatedY := rotation.RotatePosition(int(pos.X), int(pos.Y), island.Width, island.Height, currentRotation)

				centerX := float32(rotatedX) + float32(b.Size.Width())/2
				centerZ := float32(rotatedY) + float32(b.Size.Height())/2

				w := float32(tile.Image.Width) / r.PPU
				h := float32(tile.Image.Height) / r.PPU

				posY := h/2 - float32(pos.Offset)/r.PPU
				position := rl.NewVector3(centerX, posY, centerZ)
				size := rl.NewVector2(w, h)
				src := rl.NewRectangle(0, 0, float32(tile.Image.Width), float32(tile.Image.Height))

				rl.DrawBillboardRec(r.Camera, tile.Image, src, position, size, rl.White)
			}
		}
	})

	if grid {
		// TODO: implement grid rendering if needed
	}

	r.End()
}
