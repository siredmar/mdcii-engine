package systems

import (
	"bytes"
	_ "embed"
	"image"
	"image/png"
	"log"
	"sort"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/siredmar/mdcii-engine/pkg/building"
	"github.com/siredmar/mdcii-engine/pkg/cod/buildings"
	"github.com/siredmar/mdcii-engine/pkg/ecs/components"
	"github.com/siredmar/mdcii-engine/pkg/world/rotation"
	"github.com/siredmar/mdcii-engine/pkg/world/zoom"
	"github.com/yohamta/donburi"
	"github.com/yohamta/donburi/filter"
)

// Embed optional debug tiles
//
//go:embed assets/gfx/0.png
var grid0Bytes []byte

var grid0 = createImage(grid0Bytes)

func createImage(data []byte) *ebiten.Image {
	img, err := png.Decode(bytes.NewReader(data))
	if err != nil {
		log.Fatalf("Failed to decode PNG: %v", err)
	}

	rgbaImg := image.NewRGBA(img.Bounds())
	drawer := image.NewRGBA(img.Bounds())
	for y := img.Bounds().Min.Y; y < img.Bounds().Max.Y; y++ {
		for x := img.Bounds().Min.X; x < img.Bounds().Max.X; x++ {
			drawer.Set(x, y, img.At(x, y))
		}
	}
	copy(rgbaImg.Pix, drawer.Pix)

	return ebiten.NewImageFromImage(rgbaImg)
}

const (
	TILE_WIDTH  = 64
	TILE_HEIGHT = 32
)

// // Alignment offsets for multi-tile buildings
//
//	var AlignmentMap = map[building.BuildingSizeIdentifier][2]float64{
//		building.BuildingSize2x3: {-32 * 2, (32.0 / 2) * 3},
//		building.BuildingSize2x2: {-32, (32.0 / 2) * 2},
//		building.BuildingSize1x2: {-32 / 2, 32},
//		building.BuildingSize2x1: {-32 * 1, 32.0 / 2},
//		building.BuildingSize4x3: {-32 * 3, (32.0 / 2) * 5},
//	}
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

// // AlignmentOffset returns the pixel offset to correctly align multi-tile buildings
// func AlignmentOffset(size building.BuildingSizeIdentifier, rot rotation.Rotation) (float64, float64) {
// 	w, h := size.Width(), size.Height()

// 	// Anchor offset in tile-space: bottom-left (0-indexed)
// 	anchorTileX := 0
// 	anchorTileY := h - 1

// 	// Compute how far the anchor is from top-left (0,0) of the building
// 	offsetTileX := -anchorTileX
// 	offsetTileY := -anchorTileY

// 	// Rotate that offset in tile-space
// 	rotatedX, rotatedY := rotation.RotatePosition(
// 		offsetTileX, offsetTileY,
// 		w, h,
// 		rot,
// 	)

// 	// Convert to isometric pixel-space
// 	pixelX := (float64(rotatedX) - float64(rotatedY)) * (TILE_WIDTH / 2)
// 	pixelY := (float64(rotatedX) + float64(rotatedY)) * (TILE_HEIGHT / 2)

// 	return pixelX, pixelY
// }

// Renderer query
var rendererQuery = donburi.NewQuery(
	filter.Contains(components.IslandType),
)

// A renderable tile or building piece
type RenderableTile struct {
	isoX, isoY float64
	Z          float64
	topX, topY int
	Image      *ebiten.Image
}

func RenderSystem(world donburi.World, screen *ebiten.Image, grid bool, currentRotation rotation.Rotation) {
	tileWidth := zoom.TileSize()
	tileHeight := zoom.TileHeight()

	var renderableTiles []RenderableTile

	// 👉 Fetch camera (added)
	var camera *components.Camera
	cameraQuery := donburi.NewQuery(filter.Contains(components.CameraType))
	cameraQuery.Each(world, func(entry *donburi.Entry) {
		camera = components.CameraType.Get(entry)
	})
	if camera == nil {
		log.Println("No camera entity found")
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

				if tile.Image == nil {
					continue
				}

				// 🔁 Apply rotation to world position
				rotatedX, rotatedY := rotation.RotatePosition(int(pos.X), int(pos.Y), island.Width, island.Height, currentRotation)

				// 📐 Isometric projection
				isoX := ((float64(rotatedX)-island.X)-(float64(rotatedY)-island.Y))*(float64(tileWidth)/2) + float64(island.X)*(float64(tileWidth)/2)
				isoY := ((float64(rotatedX)-island.X)+(float64(rotatedY)-island.Y))*(float64(tileHeight)/2) + float64(island.Y)*(float64(tileHeight)/2)
				isoY -= pos.Offset

				// ⬇ Adjust image height for visual stacking
				tileImageHeight := float64(tile.Image.Bounds().Dy())
				isoY -= tileImageHeight - float64(tileHeight)

				// 🧭 Alignment offset (rotation-specific)
				if rotationMap, ok := AlignmentMaps[currentRotation]; ok {
					if offset, ok := rotationMap[building.Size]; ok {
						isoX += offset[0]
						isoY += offset[1]
					}
				}

				// 📏 Visual depth
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

	// 🔄 Depth sort for correct layering
	sort.Slice(renderableTiles, func(i, j int) bool {
		depthI := renderableTiles[i].topX + renderableTiles[i].topY + int(renderableTiles[i].Z)
		depthJ := renderableTiles[j].topX + renderableTiles[j].topY + int(renderableTiles[j].Z)
		return depthI < depthJ
	})

	// 🎨 Draw everything, applying camera offset
	for _, tile := range renderableTiles {
		if tile.Image != nil {
			op := &ebiten.DrawImageOptions{}
			op.GeoM.Translate(tile.isoX-camera.X, tile.isoY-camera.Y)
			screen.DrawImage(tile.Image, op)
		}
	}

	if grid {
		renderDebugGrid(world, screen, float64(tileWidth), float64(tileHeight))
	}
}

func renderDebugGrid(world donburi.World, screen *ebiten.Image, tileWidth, tileHeight float64) {
	// Implement if needed
}
