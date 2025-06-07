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

var AlignmentMap = map[buildingRotation.BuildingSizeIdentifier][2]float64{
	buildingRotation.BuildingSize2x3: {-32 * 2, (32 / 2) * 3},
	buildingRotation.BuildingSize2x2: {-32, (32 / 2) * 2},
	buildingRotation.BuildingSize1x2: {-32 / 2, 32},
	buildingRotation.BuildingSize2x1: {-32 * 1, 32 / 2},
	buildingRotation.BuildingSize4x3: {-32 * 3, (32 / 2) * 5},
}

// func RenderSystem(world donburi.World, screen *ebiten.Image, grid bool) {
// 	tileWidth := zoom.TileSize()
// 	tileHeight := zoom.TileHeight()

//		rendererQuery.Each(world, func(entry *donburi.Entry) {
//			island := components.IslandType.Get(entry)
//			for _, layerID := range []string{buildings.KindSeaID, buildings.KindGroundID, buildings.KindRoadsID, buildings.KindForrestID, buildings.KindBuildingsID} {
//				for _, tileEntry := range island.Tiles[layerID] {
//					pos := components.PositionType.Get(tileEntry)
//					tile := components.TileType.Get(tileEntry)
//					building := components.BuildingType.Get(tileEntry)
//					if tile.Image != nil {
//						// Base isometric position for Tile 1
//						isoX := ((pos.X-island.X)-(pos.Y-island.Y))*(float64(tileWidth)/2) + (island.X * (float64(tileWidth) / 2))
//						isoY := ((pos.X-island.X)+(pos.Y-island.Y))*(float64(tileHeight)/2) + (island.Y * (float64(tileHeight) / 2))
//						isoY -= pos.Offset
//						// Adjust for image height
//						tileImageHeight := float64(tile.Image.Bounds().Dy())
//						isoY -= tileImageHeight - float64(tileHeight)
//						if building.Size == buildingRotation.BuildingSize2x3 || building.Size == buildingRotation.BuildingSize2x2 || building.Size == buildingRotation.BuildingSize2x1 || building.Size == buildingRotation.BuildingSize4x3 {
//							// Apply precise alignment offsets
//							if offset, ok := AlignmentMap[building.Size]; ok {
//								isoX += offset[0] // Shift right
//								isoY += offset[1] // Shift up
//							}
//						}
//						op := &ebiten.DrawImageOptions{}
//						op.GeoM.Translate(isoX, isoY)
//						screen.DrawImage(tile.Image, op)
//					}
//				}
//			}
//		})
//	}

func RenderSystem(world donburi.World, screen *ebiten.Image, grid bool) {
	tileWidth := zoom.TileSize()
	tileHeight := zoom.TileHeight()

	type RenderableTile struct {
		isoX, isoY float64
		Z          float64
		topY       float64 // Topmost grid Y coordinate for sorting
		topX       float64 // Leftmost grid X coordinate for sorting
		Image      *ebiten.Image
	}

	var renderableTiles []RenderableTile

	rendererQuery.Each(world, func(entry *donburi.Entry) {
		island := components.IslandType.Get(entry)

		// for _, layerID := range []string{
		// 	buildings.KindSeaID,
		// 	buildings.KindGroundID,
		// 	buildings.KindRoadsID,
		// 	buildings.KindForrestID,
		// 	buildings.KindBuildingsID,
		// } {

		// first sort by y,x. fill empty spaces with empty tiles
		for _, layer := range island.Tiles {
			sort.Slice(layer, func(i, j int) bool {
				posI := components.PositionType.Get(layer[i])
				posJ := components.PositionType.Get(layer[j])
				if posI.Y == posJ.Y {
					return posI.X < posJ.X
				}
				return posI.Y < posJ.Y
			})
			// fill empty spaces with empty tiles
			for i := 0; i < len(layer)-1; i++ {
				posI := components.PositionType.Get(layer[i])
				posJ := components.PositionType.Get(layer[i+1])
				if posI.Y == posJ.Y && posI.X+1 < posJ.X {
					for x := posI.X + 1; x < posJ.X; x++ {
						tileEntity := world.Create(components.BuildingType, components.PositionType, components.TileType, components.AnimationType)
						tileEntry := world.Entry(tileEntity)
						components.PositionType.Set(tileEntry, &components.Position{X: x, Y: posI.Y})
						components.TileType.Set(tileEntry, &components.Tile{Image: nil})
						layer = append(layer, tileEntry)
					}
				}
			}
		}

		// Iterate over ALL layers
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
				// Base isometric position
				isoX := ((pos.X-island.X)-(pos.Y-island.Y))*(float64(tileWidth)/2) + (island.X * (float64(tileWidth) / 2))
				isoY := ((pos.X-island.X)+(pos.Y-island.Y))*(float64(tileHeight)/2) + (island.Y * (float64(tileHeight) / 2))
				isoY -= pos.Offset
				if tile.Occupation {
					tile.Image = grid1
				}
				if tile.Image != nil {
					// Adjust for image height
					tileImageHeight := float64(tile.Image.Bounds().Dy())
					isoY -= tileImageHeight - float64(tileHeight)
				}
				// Apply alignment offsets for multi-tile buildings
				if offset, ok := AlignmentMap[building.Size]; ok {
					isoX += offset[0]
					isoY += offset[1]
				}

				// Top-left grid reference for sorting
				topX := pos.X
				topY := pos.Y

				// Add tile to renderable list
				renderableTiles = append(renderableTiles, RenderableTile{
					isoX:  isoX,
					isoY:  isoY,
					topX:  topX, // Leftmost X for sorting
					topY:  topY, // Topmost Y for sorting
					Image: tile.Image,
					Z:     float64(tile.Size.Z),
				})
			}
		}
	})

	// Sort tiles: First by topY (topmost first), then by topX (leftmost first)
	sort.Slice(renderableTiles, func(i, j int) bool {
		if renderableTiles[i].topY == renderableTiles[j].topY {
			// If topY is the same, sort by topX (leftmost first)
			return renderableTiles[i].topX < renderableTiles[j].topX
		}
		// Sort by topY (topmost tiles first)
		return renderableTiles[i].Z > renderableTiles[j].topY
	})

	// Render tiles in sorted order
	for _, tile := range renderableTiles {
		if tile.Image != nil {
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

func renderDebugGrid(world donburi.World, screen *ebiten.Image, tileWidth, tileHeight float64) {
}
