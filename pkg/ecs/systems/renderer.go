package systems

import (
	"bytes"
	_ "embed"
	"image/png"
	"log"
	"sort"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/siredmar/mdcii-engine/pkg/cod/buildings"
	"github.com/siredmar/mdcii-engine/pkg/ecs/components"
	"github.com/siredmar/mdcii-engine/pkg/texture/mapping"
	"github.com/siredmar/mdcii-engine/pkg/world/rotation"
	"github.com/siredmar/mdcii-engine/pkg/world/zoom"
	"github.com/yohamta/donburi"
	"github.com/yohamta/donburi/filter"
)

//go:embed assets/gfx/0.png
var grid0Bytes []byte

var grid0 = createImage(grid0Bytes)

func createImage(data []byte) *ebiten.Image {
	img, err := png.Decode(bytes.NewReader(data))
	if err != nil {
		log.Fatalf("Failed to decode PNG: %v", err)
	}
	return ebiten.NewImageFromImage(img)
}

const (
	TILE_WIDTH  = 64
	TILE_HEIGHT = 32
)

var rendererQuery = donburi.NewQuery(filter.Contains(components.IslandType))

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

				if tile == nil || tile.Image == nil || tile.Metadata == nil {
					return
				}

				// Rotate position
				rotatedX, rotatedY := rotation.RotatePosition(int(pos.X), int(pos.Y), island.Width, island.Height, currentRotation)

				// Convert to isometric coordinates
				isoX := ((float64(rotatedX)-island.X)-(float64(rotatedY)-island.Y))*(float64(tileWidth)/2) + float64(island.X)*(float64(tileWidth)/2)
				isoY := ((float64(rotatedX)-island.X)+(float64(rotatedY)-island.Y))*(float64(tileHeight)/2) + float64(island.Y)*(float64(tileHeight)/2)
				isoY -= pos.Offset

				tileImageHeight := float64(tile.Image.Bounds().Dy())
				isoY -= tileImageHeight - float64(tileHeight)

				// Compute anchor offset using GetAnchorOffset
				anchorTileX, anchorTileY := mapping.GetAnchorOffset(building.Size, currentRotation)
				anchorPixelX := float64(anchorTileX-anchorTileY) * (float64(tileWidth) / 2)
				anchorPixelY := float64(anchorTileX+anchorTileY) * (float64(tileHeight) / 2)
				isoX -= anchorPixelX
				isoY -= anchorPixelY

				// Use anchor pixel offset from atlas metadata for correct alignment
				meta := tile.Metadata
				isoX -= float64(meta.AnchorPixelX)
				isoY -= float64(meta.AnchorPixelY)

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
	// optional grid rendering
}
