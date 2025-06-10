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
	"github.com/siredmar/mdcii-engine/pkg/world/zoom"
	"github.com/yohamta/donburi"
	"github.com/yohamta/donburi/filter"
)

//go:embed assets/gfx/0.png
var grid0Bytes []byte

//go:embed assets/gfx/1.png
var grid1Tile []byte

var grid0 = createImage(grid0Bytes)

// var grid1 = createImage(grid1Tile)

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

// Alignment for various building sizes
var AlignmentMap = map[building.BuildingSizeIdentifier][2]float64{
	building.BuildingSize2x3: {-32 * 2, (32.0 / 2) * 3},
	building.BuildingSize2x2: {-32, (32.0 / 2) * 2},
	building.BuildingSize1x2: {-32 / 2, 32},
	building.BuildingSize2x1: {-32 * 1, 32.0 / 2},
	building.BuildingSize4x3: {-32 * 3, (32.0 / 2) * 5},
}

// Renderer ECS query
var rendererQuery = donburi.NewQuery(
	filter.Contains(components.IslandType),
)

type RenderableTile struct {
	isoX, isoY float64
	Z          float64
	topX, topY float64
	Image      *ebiten.Image
}

func RenderSystem(world donburi.World, screen *ebiten.Image, grid bool) {
	tileWidth := zoom.TileSize()
	tileHeight := zoom.TileHeight()

	var renderableTiles []RenderableTile

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

				// if tile.Image == nil && tile.Occupation {
				// 	tile.Image = grid1
				// }
				if tile.Image == nil {
					continue
				}

				// Compute screen coordinates
				isoX := ((pos.X-island.X)-(pos.Y-island.Y))*(float64(tileWidth)/2) + float64(island.X)*(float64(tileWidth)/2)
				isoY := ((pos.X-island.X)+(pos.Y-island.Y))*(float64(tileHeight)/2) + float64(island.Y)*(float64(tileHeight)/2)
				isoY -= pos.Offset

				// Adjust image height
				tileImageHeight := float64(tile.Image.Bounds().Dy())
				isoY -= tileImageHeight - float64(tileHeight)

				// Apply alignment offset if needed
				if offset, ok := AlignmentMap[building.Size]; ok {
					isoX += offset[0]
					isoY += offset[1]
				}

				// Calculate visual Z-depth from image height
				visualZ := tileImageHeight / float64(tileHeight)

				renderableTiles = append(renderableTiles, RenderableTile{
					isoX:  isoX,
					isoY:  isoY,
					topX:  pos.X,
					topY:  pos.Y,
					Z:     visualZ,
					Image: tile.Image,
				})
			}
		}
	})

	// Sort tiles by isometric depth order
	sort.Slice(renderableTiles, func(i, j int) bool {
		depthI := renderableTiles[i].topX + renderableTiles[i].topY + renderableTiles[i].Z
		depthJ := renderableTiles[j].topX + renderableTiles[j].topY + renderableTiles[j].Z
		return depthI < depthJ
	})

	for _, tile := range renderableTiles {
		if tile.Image != nil {
			op := &ebiten.DrawImageOptions{}
			op.GeoM.Translate(tile.isoX, tile.isoY)
			screen.DrawImage(tile.Image, op)
		}
	}

	if grid {
		renderDebugGrid(world, screen, float64(tileWidth), float64(tileHeight))
	}
}

func renderDebugGrid(world donburi.World, screen *ebiten.Image, tileWidth, tileHeight float64) {
	// Optional debug grid overlay
}
