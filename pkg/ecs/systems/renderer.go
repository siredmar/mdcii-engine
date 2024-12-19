package systems

import (
	"bytes"
	_ "embed"
	"image"
	"image/png"
	"log"
	"sort"

	"github.com/hajimehoshi/ebiten/v2"
	buildingRotation "github.com/siredmar/mdcii-engine/pkg/building"
	"github.com/siredmar/mdcii-engine/pkg/cod/buildings"
	"github.com/siredmar/mdcii-engine/pkg/ecs/components"
	"github.com/siredmar/mdcii-engine/pkg/world/zoom"
	"github.com/yohamta/donburi"
	"github.com/yohamta/donburi/filter"
)

// Define a query using filter.LayoutFilter
var rendererQuery = donburi.NewQuery(
	filter.Contains(components.IslandType),
)

//go:embed assets/gfx/0.png
var grid0Bytes []byte

//go:embed assets/gfx/1.png
var grid1Tile []byte

func createImage(data []byte) *ebiten.Image {
	img, err := png.Decode(bytes.NewReader(data))
	if err != nil {
		log.Fatalf("Failed to decode PNG: %v", err)
	}

	// Convert image.Image to *image.RGBA
	rgba, ok := img.(*image.RGBA)
	if !ok {
		// If the image isn't already RGBA, we need to create a new RGBA image and draw onto it
		bounds := img.Bounds()
		rgba = image.NewRGBA(bounds)
		for y := bounds.Min.Y; y < bounds.Max.Y; y++ {
			for x := bounds.Min.X; x < bounds.Max.X; x++ {
				rgba.Set(x, y, img.At(x, y))
			}
		}
	}

	return ebiten.NewImageFromImage(rgba)
}

var grid0 = createImage(grid0Bytes)
var grid1 = createImage(grid1Tile)

func renderDebugGrid(world donburi.World, screen *ebiten.Image, w, h float64) {

}

type RenderableTile struct {
	isoX, isoY       float64
	bottomX, bottomY float64 // Bottom-right grid position for sorting
	Z                int
	HighFlag         bool
	Image            *ebiten.Image
	Occupy           bool
	Width, Height    int
}

var AlignmentMap = map[buildingRotation.BuildingSizeIdentifier][2]float64{
	buildingRotation.BuildingSize2x3: {-32 * 2, (32 / 2) * 3},
	buildingRotation.BuildingSize2x2: {-32, (32 / 2) * 2},
	buildingRotation.BuildingSize1x2: {-32 / 2, 32},
	buildingRotation.BuildingSize2x1: {-32 * 1, 32 / 2},
	buildingRotation.BuildingSize4x3: {-32 * 3, (32 / 2) * 5},
}

func RenderSystem(world donburi.World, screen *ebiten.Image, grid bool) {
	tileWidth := zoom.TileSize()
	tileHeight := zoom.TileHeight()

	type RenderableTile struct {
		isoX, isoY       float64
		bottomX, bottomY float64
		depth            float64 // Calculated depth score
		Z                int
		HighFlag         bool
		Image            *ebiten.Image
		Occupy           bool
		Width, Height    int
	}

	var renderableTiles []RenderableTile

	rendererQuery.Each(world, func(entry *donburi.Entry) {
		island := components.IslandType.Get(entry)

		for _, layerID := range []string{
			buildings.KindBuildingsID,
			buildings.KindForrestID,
			buildings.KindGroundID,
			buildings.KindRoadsID,
			buildings.KindSeaID,
		} {
			for _, tileEntry := range island.Tiles[layerID] {
				pos := components.PositionType.Get(tileEntry)
				tile := components.TileType.Get(tileEntry)
				building := components.BuildingType.Get(tileEntry)

				// Base isometric position
				isoX := ((pos.X-island.X)-(pos.Y-island.Y))*(float64(tileWidth)/2) + (island.X * (float64(tileWidth) / 2))
				isoY := ((pos.X-island.X)+(pos.Y-island.Y))*(float64(tileHeight)/2) + (island.Y * (float64(tileHeight) / 2))
				isoY -= pos.Offset

				if tile.Image != nil {
					// Adjust for image height
					tileImageHeight := float64(tile.Image.Bounds().Dy())
					isoY -= tileImageHeight - float64(tileHeight)
				}

				// Apply alignment offsets
				if offset, ok := AlignmentMap[building.Size]; ok {
					isoX += offset[0]
					isoY += offset[1]
				}

				// Bottom grid reference for sorting
				bottomX := pos.X + float64(tile.Size.Width-1)
				bottomY := pos.Y + float64(tile.Size.Height-1)

				// Calculate depth score
				depth := (1000 * bottomY) + bottomX + (10 * boolToFloat(tile.Size.Z > 0)) - float64(tile.Size.Z)

				renderableTiles = append(renderableTiles, RenderableTile{
					isoX:     isoX,
					isoY:     isoY,
					bottomX:  bottomX,
					bottomY:  bottomY,
					depth:    depth,
					Image:    tile.Image,
					Z:        tile.Size.Z,
					HighFlag: tile.Size.Z > 0, // Use Z > 0 as HighFlag
					Occupy:   tile.Occupation,
					Width:    tile.Size.Width,
					Height:   tile.Size.Height,
				})
			}
		}
	})

	// Sorting tiles
	sort.Slice(renderableTiles, func(i, j int) bool {
		// First compare HighFlag (buildings first)
		if renderableTiles[i].HighFlag != renderableTiles[j].HighFlag {
			return renderableTiles[i].HighFlag
		}
		// Compare bottomY
		if renderableTiles[i].bottomY != renderableTiles[j].bottomY {
			return renderableTiles[i].bottomY < renderableTiles[j].bottomY
		}
		// Compare bottomX
		return renderableTiles[i].bottomX < renderableTiles[j].bottomX
	})

	// Render tiles in sorted order
	for _, tile := range renderableTiles {
		if tile.Image != nil && !tile.Occupy {
			op := &ebiten.DrawImageOptions{}
			op.GeoM.Translate(tile.isoX, tile.isoY)
			screen.DrawImage(tile.Image, op)
		}
	}

	// Optional debug grid overlay
	if grid {
		renderDebugGrid(world, screen, float64(tileWidth), float64(tileHeight))
	}
}

func boolToFloat(b bool) float64 {
	if b {
		return 1.0
	}
	return 0.0
}
func AlignmentMapOffset(width, height int) float64 {
	if offset, ok := AlignmentMap[buildingRotation.BuildingSize(width, height)]; ok {
		return offset[1] // Return the vertical offset
	}
	return 0 // Default: No offset
}
