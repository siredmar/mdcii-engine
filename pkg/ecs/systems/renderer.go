package systems

import (
	"sort"

	rl "github.com/gen2brain/raylib-go/raylib"
	"github.com/siredmar/mdcii-engine/pkg/building"
	"github.com/siredmar/mdcii-engine/pkg/cod/buildings"
	"github.com/siredmar/mdcii-engine/pkg/ecs/components"
	"github.com/siredmar/mdcii-engine/pkg/world/rotation"
	"github.com/siredmar/mdcii-engine/pkg/world/zoom"
	"github.com/yohamta/donburi"
	"github.com/yohamta/donburi/filter"
)

const (
	TILE_WIDTH  = 64
	TILE_HEIGHT = 32
)

var AlignmentMaps = map[rotation.Rotation]map[building.BuildingSizeIdentifier][2]float64{
	rotation.DEG0: {
		building.BuildingSize2x2: {-32, 32},
		building.BuildingSize2x3: {-64, 48},
		building.BuildingSize1x2: {-16, 32},
		building.BuildingSize2x1: {-32, 16},
		building.BuildingSize4x3: {-96, 80},
	},
	rotation.DEG90: {
		building.BuildingSize2x2: {-32, 32},
		building.BuildingSize2x3: {-48, 64},
		building.BuildingSize1x2: {-16, 32},
		building.BuildingSize2x1: {-32, 16},
		building.BuildingSize4x3: {-80, 96},
	},
	rotation.DEG180: {
		building.BuildingSize2x2: {-32, 32},
		building.BuildingSize2x3: {-64, 48},
		building.BuildingSize1x2: {-16, 32},
		building.BuildingSize2x1: {-32, 16},
		building.BuildingSize4x3: {-96, 80},
	},
	rotation.DEG270: {
		building.BuildingSize2x2: {-32, 32},
		building.BuildingSize2x3: {-48, 64},
		building.BuildingSize1x2: {-16, 32},
		building.BuildingSize2x1: {-32, 16},
		building.BuildingSize4x3: {-80, 96},
	},
}

var rendererQuery = donburi.NewQuery(
	filter.Contains(components.IslandType),
)

type RenderableTile struct {
	isoX, isoY float64
	Z          float64
	topX, topY int
	Image      rl.Texture2D
}

func RenderSystem(world donburi.World, grid bool, currentRotation rotation.Rotation) {
	tileWidth := zoom.TileSize()
	tileHeight := zoom.TileHeight()

	var renderableTiles []RenderableTile

	var camera *components.Camera
	cameraQuery := donburi.NewQuery(filter.Contains(components.CameraType))
	cameraQuery.Each(world, func(entry *donburi.Entry) {
		camera = components.CameraType.Get(entry)
	})
	if camera == nil {
		return
	}

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
				building := components.BuildingType.Get(tileEntry)

				if tile.Image.ID == 0 {
					continue
				}

				rotatedX, rotatedY := rotation.RotatePosition(int(pos.X), int(pos.Y), island.Width, island.Height, currentRotation)

				isoX := ((float64(rotatedX)-island.X)-(float64(rotatedY)-island.Y))*(float64(tileWidth)/2) + float64(island.X)*(float64(tileWidth)/2)
				isoY := ((float64(rotatedX)-island.X)+(float64(rotatedY)-island.Y))*(float64(tileHeight)/2) + float64(island.Y)*(float64(tileHeight)/2)
				isoY -= pos.Offset

				tileImageHeight := float64(tile.Image.Height)
				isoY -= tileImageHeight - float64(tileHeight)

				if rotationMap, ok := AlignmentMaps[currentRotation]; ok {
					if offset, ok := rotationMap[building.Size]; ok {
						isoX += offset[0]
						isoY += offset[1]
					}
				}

				visualZ := tileImageHeight / float64(tileHeight)

				renderableTiles = append(renderableTiles, RenderableTile{
					isoX:  isoX,
					isoY:  isoY,
					topX:  rotatedX,
					topY:  rotatedY,
					Z:     visualZ,
					Image: tile.Image,
				})
			}
		}
	})

	sort.Slice(renderableTiles, func(i, j int) bool {
		depthI := renderableTiles[i].topX + renderableTiles[i].topY + int(renderableTiles[i].Z)
		depthJ := renderableTiles[j].topX + renderableTiles[j].topY + int(renderableTiles[j].Z)
		return depthI < depthJ
	})

	for _, tile := range renderableTiles {
		pos := rl.NewVector2(float32(tile.isoX-camera.X), float32(tile.isoY-camera.Y))
		rl.DrawTextureV(tile.Image, pos, rl.White)
	}

	if grid {
		renderDebugGrid(world, float64(tileWidth), float64(tileHeight))
	}
}

func renderDebugGrid(world donburi.World, tileWidth, tileHeight float64) {
	// TODO: implement grid rendering if needed
}
