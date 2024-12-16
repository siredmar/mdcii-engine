package systems

import (
	"bytes"
	_ "embed"
	"image"
	"image/png"
	"log"

	"github.com/hajimehoshi/ebiten/v2"
	buildingRotation "github.com/siredmar/mdcii-engine/pkg/building"
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

// func RenderSystem(world donburi.World, screen *ebiten.Image, grid bool) {
// 	tileWidth := zoom.TileSize()
// 	tileHeight := zoom.TileHeight()
// 	rendererQuery.Each(world, func(entry *donburi.Entry) {
// 		island := components.IslandType.Get(entry)
// 		for _, tileEntry := range island.Tiles {
// 			pos := components.PositionType.Get(tileEntry)
// 			tile := components.TileType.Get(tileEntry)
// 			if tile.Image != nil {
// 				isoX := ((pos.X-island.X)-(pos.Y-island.Y))*(float64(tileWidth)/2) + (island.X * (float64(tileWidth) / 2))
// 				isoY := ((pos.X-island.X)+(pos.Y-island.Y))*(float64(tileHeight)/2) + (island.Y * (float64(tileHeight) / 2))
// 				isoY -= pos.Offset

// 				// Adjust Y position so the bottom of the tile aligns
// 				// Assuming tile.Image is an *ebiten.Image, we can get its height
// 				tileImageHeight := float64(tile.Image.Bounds().Dy())
// 				isoY -= tileImageHeight - float64(tileHeight)

//					op := &ebiten.DrawImageOptions{}
//					op.GeoM.Translate(isoX, isoY)
//					screen.DrawImage(tile.Image, op)
//					if tile.Size.Height > 1 && tile.Size.Width > 1 {
//						// draw corner of bigger tiles than 1x1 to see where they start
//						gridOp := &ebiten.DrawImageOptions{}
//						gridOp.GeoM.Translate(isoX, isoY)
//						screen.DrawImage(grid1, gridOp)
//					}
//				}
//			}
//			if grid {
//				for _, tileEntry := range island.Tiles {
//					pos := components.PositionType.Get(tileEntry)
//					tile := components.TileType.Get(tileEntry)
//					if tile.Image != nil {
//						// Calculate isometric position
//						isoX := ((pos.X-island.X)-(pos.Y-island.Y))*(float64(tileWidth)/2) + (island.X * (float64(tileWidth) / 2))
//						isoY := ((pos.X-island.X)+(pos.Y-island.Y))*(float64(tileHeight)/2) + (island.Y * (float64(tileHeight) / 2))
//						// Draw grid
//						gridOp := &ebiten.DrawImageOptions{}
//						gridOp.GeoM.Translate(isoX, isoY)
//						screen.DrawImage(grid0, gridOp)
//					}
//				}
//			}
//		})
//	}

// func RenderSystem(world donburi.World, screen *ebiten.Image, grid bool) {
// 	tileWidth := zoom.TileSize()
// 	tileHeight := zoom.TileHeight()

// 	// Predefined anchor offsets per rotation
// 	var anchorOffsets = map[rotation.Rotation][2]float64{
// 		rotation.DEG0:   {-1, 0}, // Rotation 0
// 		rotation.DEG90:  {-2, 0}, // Rotation 1: Shift right
// 		rotation.DEG180: {-3, 0}, // Rotation 2: Shift down
// 		rotation.DEG270: {1, 2},  // Rotation 3: Shift left
// 	}

// 	// Iterate through all renderable entities
// 	rendererQuery.Each(world, func(entry *donburi.Entry) {
// 		island := components.IslandType.Get(entry)

// 		for _, tileEntry := range island.Tiles {
// 			pos := components.PositionType.Get(tileEntry)
// 			tile := components.TileType.Get(tileEntry)
// 			building := components.BuildingType.Get(tileEntry)

// 			if tile.Image != nil {
// 				adjX := pos.X
// 				adjY := pos.Y

// 				if tile.Size.Width == 2 || tile.Size.Height == 2 {
// 					// Determine anchor offset for the current rotation
// 					offset := anchorOffsets[building.Rotation]

// 					// Adjust grid position with the anchor offset
// 					adjX = pos.X + offset[0]
// 					adjY = pos.Y + offset[1]

// 				}
// 				// Convert grid position to isometric coordinates
// 				isoX := ((adjX - float64(island.X)) - (adjY - float64(island.Y))) * (float64(tileWidth) / 2)
// 				isoY := ((adjX - float64(island.X)) + (adjY - float64(island.Y))) * (float64(tileHeight) / 2)

// 				// Align image Y-offset for proper rendering
// 				imageHeight := float64(tile.Image.Bounds().Dy())
// 				isoY -= imageHeight - float64(tileHeight)

// 				// Apply vertical offset if specified
// 				isoY -= pos.Offset

// 				// Draw the building/tile
// 				op := &ebiten.DrawImageOptions{}
// 				op.GeoM.Translate(isoX, isoY)
// 				screen.DrawImage(tile.Image, op)

// 				// Debug: Draw grid indicator for multi-tile buildings
// 				if tile.Size.Width > 1 || tile.Size.Height > 1 {
// 					gridOp := &ebiten.DrawImageOptions{}
// 					gridOp.GeoM.Translate(isoX, isoY)
// 					screen.DrawImage(grid1, gridOp) // grid1 is a debug overlay
// 				}
// 			}
// 		}

// 		// Render grid overlay if enabled
// 		if grid {
// 			for _, tileEntry := range island.Tiles {
// 				pos := components.PositionType.Get(tileEntry)

// 				isoX := ((float64(pos.X-island.X) - float64(pos.Y-island.Y)) * (float64(tileWidth) / 2))
// 				isoY := ((float64(pos.X-island.X) + float64(pos.Y-island.Y)) * (float64(tileHeight) / 2))

// 				gridOp := &ebiten.DrawImageOptions{}
// 				gridOp.GeoM.Translate(isoX, isoY)
// 				screen.DrawImage(grid0, gridOp) // grid0 is a debug grid image
// 			}
// 		}
// 	})
// }

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

	// Temporary list for sorting tiles by depth
	// type RenderableTile struct {
	// 	isoX, isoY float64
	// 	image      *ebiten.Image
	// }

	// var renderableTiles []RenderableTile

	rendererQuery.Each(world, func(entry *donburi.Entry) {
		island := components.IslandType.Get(entry)

		for _, tileEntry := range island.Tiles {
			pos := components.PositionType.Get(tileEntry)
			tile := components.TileType.Get(tileEntry)
			building := components.BuildingType.Get(tileEntry)
			if tile.Image != nil {
				// Base isometric position for Tile 1
				isoX := ((pos.X-island.X)-(pos.Y-island.Y))*(float64(tileWidth)/2) + (island.X * (float64(tileWidth) / 2))
				isoY := ((pos.X-island.X)+(pos.Y-island.Y))*(float64(tileHeight)/2) + (island.Y * (float64(tileHeight) / 2))
				isoY -= pos.Offset
				// Adjust for image height
				tileImageHeight := float64(tile.Image.Bounds().Dy())
				isoY -= tileImageHeight - float64(tileHeight)
				if building.Size == buildingRotation.BuildingSize2x3 || building.Size == buildingRotation.BuildingSize2x2 || building.Size == buildingRotation.BuildingSize2x1 || building.Size == buildingRotation.BuildingSize4x3 {
					// Apply precise alignment offsets
					if offset, ok := AlignmentMap[building.Size]; ok {
						isoX += offset[0] // Shift right
						isoY += offset[1] // Shift up
					}
				}

				if building.Size != buildingRotation.BuildingSize1x1 {
					// Render tiles in sorted order
					// for _, tile := range renderableTiles {
					op := &ebiten.DrawImageOptions{}
					op.GeoM.Translate(isoX, isoY)
					screen.DrawImage(tile.Image, op)
				}
				// }

				// // Add tile to the list for sorting
				// renderableTiles = append(renderableTiles, RenderableTile{
				// 	isoX:  isoX,
				// 	isoY:  isoY,
				// 	image: tile.Image,
				// })
			}
		}
	})

	// // Sort renderable tiles by Y-coordinate for proper draw order
	// sort.Slice(renderableTiles, func(i, j int) bool {
	// 	// First, sort by the sum of grid coordinates (diagonal order)
	// 	sumI := renderableTiles[i].isoX + renderableTiles[i].isoY
	// 	sumJ := renderableTiles[j].isoX + renderableTiles[j].isoY

	// 	if sumI == sumJ {
	// 		// If diagonal sums are equal, sort by X (leftmost first)
	// 		return renderableTiles[i].isoX < renderableTiles[j].isoX
	// 	}

	// 	return sumI < sumJ
	// })

	// Optional: Debug grid overlay
	// if grid {
	// 	renderDebugGrid(world, screen, float64(tileWidth), float64(tileHeight))
	// }
}

// func renderDebugGrid(world donburi.World, screen *ebiten.Image, tileWidth, tileHeight float64) {
// 	rendererQuery.Each(world, func(entry *donburi.Entry) {
// 		island := components.IslandType.Get(entry)

// 		// Loop through all tiles in the island grid
// 		for y := 0; y < island.Height; y++ {
// 			for x := 0; x < island.Width; x++ {
// 				// Calculate isometric position for grid lines
// 				isoX := ((float64(x) - float64(island.X)) - (float64(y) - float64(island.Y))) * (tileWidth / 2)
// 				isoY := ((float64(x) - float64(island.X)) + (float64(y) - float64(island.Y))) * (tileHeight / 2)

// 				// Draw the grid marker (e.g., a red square or debug point)
// 				op := &ebiten.DrawImageOptions{}
// 				op.GeoM.Translate(isoX, isoY)
// 				screen.DrawImage(grid0, op) // Use a small debug tile image, `grid0`
// 			}
// 		}
// 	})
// }
