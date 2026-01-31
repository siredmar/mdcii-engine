package systems

import (
	"fmt"
	"image/color"
	"log"
	"math"
	"sort"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/hajimehoshi/ebiten/v2/text"
	"github.com/siredmar/mdcii-engine/pkg/cod/buildings"
	"github.com/siredmar/mdcii-engine/pkg/ecs/components"
	"github.com/siredmar/mdcii-engine/pkg/world/rotation"
	"github.com/siredmar/mdcii-engine/pkg/world/zoom"
	"github.com/yohamta/donburi"
	"github.com/yohamta/donburi/filter"
	"golang.org/x/image/font/basicfont"
)

const (
	TILE_WIDTH  = 64
	TILE_HEIGHT = 32
)

var rendererQuery = donburi.NewQuery(filter.Contains(components.IslandType))

type RenderableTile struct {
	isoX, isoY     float64
	originX        float64
	originY        float64
	bottomY        float64
	topX, topY     int
	tileW, tileH   int // Building footprint size for depth sorting
	Image          *ebiten.Image
	Layer          int
	pivotX         float64
	pivotY         float64
	SpriteRotation int // 0-3: number of 90° clockwise rotations to apply
}

func RenderSystem(world donburi.World, screen *ebiten.Image, grid bool, currentRotation rotation.Rotation) {
	tileWidth := zoom.TileSize()
	tileHeight := zoom.TileHeight()

	var renderableTiles []RenderableTile

	var camera *components.Camera
	var overlay bool
	cameraQuery := donburi.NewQuery(filter.Contains(components.CameraType))
	cameraQuery.Each(world, func(entry *donburi.Entry) {
		camera = components.CameraType.Get(entry)
	})
	controlQuery := donburi.NewQuery(filter.Contains(components.ControlType))
	controlQuery.Each(world, func(entry *donburi.Entry) {
		overlay = components.ControlType.Get(entry).OverlayVisible
	})
	if camera == nil {
		log.Println("No camera entity found")
		return
	}

	rendererQuery.Each(world, func(entry *donburi.Entry) {
		island := components.IslandType.Get(entry)

		// Draw infinite sea background (outside island bounds) to match original game view.
		var seaImg *ebiten.Image
		var seaPivotX, seaPivotY float64
		for _, tileEntry := range island.Tiles[buildings.KindSeaID] {
			tile := components.TileType.Get(tileEntry)
			if tile.Image == nil {
				continue
			}
			b := components.BuildingType.Get(tileEntry)
			if b != nil && b.BuildingID == 1201 { // deep sea tile
				seaImg = tile.Image
				seaPivotX = float64(tile.PivotX)
				seaPivotY = float64(tile.PivotY)
				break
			}
			if seaImg == nil {
				seaImg = tile.Image
				seaPivotX = float64(tile.PivotX)
				seaPivotY = float64(tile.PivotY)
			}
		}
		drawSeaBackground(screen, camera, island, seaImg, seaPivotX, seaPivotY, tileWidth, tileHeight)

		// Hide underlying layers under buildings based on building footprint occupancy.
		occupied := map[[2]int]struct{}{}
		for _, tileEntry := range island.Tiles[buildings.KindBuildingsID] {
			pos := components.PositionType.Get(tileEntry)
			tile := components.TileType.Get(tileEntry)
			if tile.Occupation {
				occupied[[2]int{int(pos.X), int(pos.Y)}] = struct{}{}
			}
		}

		// Also mark road positions as occupied to hide forest tiles under roads
		roadPositions := map[[2]int]struct{}{}
		for _, tileEntry := range island.Tiles[buildings.KindRoadsID] {
			pos := components.PositionType.Get(tileEntry)
			roadPositions[[2]int{int(pos.X), int(pos.Y)}] = struct{}{}
		}

		// Some tiles (notably roads) are self-contained (they include their own base ground).
		// Only skip them if we know we will still draw a base tile (ground) underneath.
		base := map[[2]int]struct{}{}
		for _, layerID := range []string{buildings.KindGroundID, buildings.KindGroundID + "_OVERLAY"} {
			for _, tileEntry := range island.Tiles[layerID] {
				pos := components.PositionType.Get(tileEntry)
				tile := components.TileType.Get(tileEntry)
				if tile.Image == nil {
					continue
				}
				base[[2]int{int(pos.X), int(pos.Y)}] = struct{}{}
			}
		}

		layerOrder := []string{
			buildings.KindSeaID,                 // Contains Surf, Estuary - plain Sea tiles filtered out below
			buildings.KindGroundID + "_OVERLAY", // Ground underlay for slope/cliff tiles
			buildings.KindGroundID,
			buildings.KindRoadsID,
			// Forest and Buildings share same layer priority for proper depth interleaving
			buildings.KindForrestID,
			buildings.KindBuildingsID,
		}

		// Layer indices for sorting - forest and buildings share same priority
		layerPriority := map[string]int{
			buildings.KindSeaID:                 0,
			buildings.KindGroundID + "_OVERLAY": 1,
			buildings.KindGroundID:              2,
			buildings.KindRoadsID:               3,
			buildings.KindForrestID:             4, // Same priority as buildings
			buildings.KindBuildingsID:           4, // Same priority as forest
		}

		for _, layerID := range layerOrder {
			for _, tileEntry := range island.Tiles[layerID] {
				pos := components.PositionType.Get(tileEntry)
				tile := components.TileType.Get(tileEntry)

				// Skip forest tiles that overlap with buildings or roads
				if layerID == buildings.KindForrestID {
					posKey := [2]int{int(pos.X), int(pos.Y)}
					if _, onBuilding := occupied[posKey]; onBuilding {
						continue
					}
					if _, onRoad := roadPositions[posKey]; onRoad {
						continue
					}
				}

				// Skip roads only if they overlap building footprints AND there's ground underneath
				if layerID == buildings.KindRoadsID {
					if _, ok := occupied[[2]int{int(pos.X), int(pos.Y)}]; ok {
						if _, hasBase := base[[2]int{int(pos.X), int(pos.Y)}]; hasBase {
							continue
						}
					}
				}

				if tile.Image == nil {
					continue
				}

				// Rotate position (RotatePosition expects local island coords)
				lx := int(pos.X - island.X)
				ly := int(pos.Y - island.Y)
				rxl, ryl := rotation.RotatePosition(lx, ly, island.Width, island.Height, currentRotation)
				rotatedX, rotatedY := rxl+int(island.X), ryl+int(island.Y)

				// Tile origin convention: (x,y) maps to the top point of the isometric diamond.
				originX := ((float64(rotatedX)-island.X)-(float64(rotatedY)-island.Y))*(float64(tileWidth)/2) + float64(island.X)*(float64(tileWidth)/2)
				originY := ((float64(rotatedX)-island.X)+(float64(rotatedY)-island.Y))*(float64(tileHeight)/2) + float64(island.Y)*(float64(tileHeight)/2)
				originY -= pos.Offset

				drawX := originX - float64(tile.PivotX)
				drawY := originY - float64(tile.PivotY)

				bottomY := drawY + float64(tile.Image.Bounds().Dy())
				renderableTiles = append(renderableTiles, RenderableTile{
					isoX:           drawX,
					isoY:           drawY,
					originX:        originX,
					originY:        originY,
					bottomY:        bottomY,
					topX:           rotatedX,
					topY:           rotatedY,
					tileW:          tile.Size.Width,
					tileH:          tile.Size.Height,
					Image:          tile.Image,
					Layer:          layerPriority[layerID],
					pivotX:         float64(tile.PivotX),
					pivotY:         float64(tile.PivotY),
					SpriteRotation: tile.SpriteRotation,
				})
			}
		}
	})

	sort.Slice(renderableTiles, func(i, j int) bool {
		a, b := renderableTiles[i], renderableTiles[j]
		// First sort by layer - sea before ground before buildings
		if a.Layer != b.Layer {
			return a.Layer < b.Layer
		}

		// Isometric depth sorting using grid coordinates.
		// For multi-tile buildings, we need to consider which tiles they occupy.
		// A 1x1 tile at (x,y) should be drawn AFTER a multi-tile building if
		// (x,y) is "in front of" (higher x+y than) any tile the building occupies.

		// Calculate the "front" depth for each tile (front corner = anchor + size - 1)
		aFrontDepth := a.topX + a.topY + (a.tileW - 1) + (a.tileH - 1)
		bFrontDepth := b.topX + b.topY + (b.tileW - 1) + (b.tileH - 1)

		// Sort by front depth - tiles with higher front depth are drawn later (in front)
		if aFrontDepth != bFrontDepth {
			return aFrontDepth < bFrontDepth
		}
		// Tie-breaker: topX (right side drawn later)
		return a.topX < b.topX
	})

	for _, tile := range renderableTiles {
		if tile.Image == nil {
			continue
		}
		op := &ebiten.DrawImageOptions{}

		// Apply sprite rotation for tiles with Rotate=0 in COD but non-zero orientation
		if tile.SpriteRotation > 0 {
			w, h := float64(tile.Image.Bounds().Dx()), float64(tile.Image.Bounds().Dy())
			// Rotate around image center
			op.GeoM.Translate(-w/2, -h/2)
			op.GeoM.Rotate(float64(tile.SpriteRotation) * math.Pi / 2)
			op.GeoM.Translate(w/2, h/2)
		}

		op.GeoM.Translate(tile.isoX-camera.X, tile.isoY-camera.Y)
		screen.DrawImage(tile.Image, op)

		if overlay {
			x := int(tile.originX - camera.X)
			y := int(tile.originY - camera.Y)
			text.Draw(screen, "+", basicfont.Face7x13, x-3, y+5, color.RGBA{255, 255, 255, 255})
			text.Draw(screen, fmt.Sprintf("%d", tile.Layer), basicfont.Face7x13, x+6, y+5, color.RGBA{255, 255, 0, 255})
		}
	}

	if grid {
		renderDebugGrid(world, screen, float64(tileWidth), float64(tileHeight))
	}
}

func renderDebugGrid(world donburi.World, screen *ebiten.Image, tileWidth, tileHeight float64) {
	// Disabled for now - uncomment to show coordinate labels on tiles
	/*
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
			for ly := 0; ly < island.Height; ly++ {
				for lx := 0; lx < island.Width; lx++ {
					// Calculate screen position for this tile
					originX := ((float64(lx))-(float64(ly)))*(tileWidth/2) + island.X*(tileWidth/2)
					originY := ((float64(lx))+(float64(ly)))*(tileHeight/2) + island.Y*(tileHeight/2)
					sx := int(originX - camera.X)
					sy := int(originY - camera.Y)
					// Draw coordinate text
					label := fmt.Sprintf("%d,%d", lx, ly)
					text.Draw(screen, label, basicfont.Face7x13, sx-10, sy+5, color.RGBA{255, 255, 0, 200})
				}
			}
		})
	*/
}

func drawSeaBackground(screen *ebiten.Image, camera *components.Camera, island *components.Island, seaImg *ebiten.Image, pivotX, pivotY float64, tileWidth, tileHeight int) {
	// if seaImg == nil || camera == nil || island == nil {
	// 	return
	// }

	// sw, sh := screen.Bounds().Dx(), screen.Bounds().Dy()
	// marginX := float64(tileWidth)
	// marginY := float64(tileHeight)
	// minWX := camera.X - marginX
	// minWY := camera.Y - marginY
	// maxWX := camera.X + float64(sw) + marginX
	// maxWY := camera.Y + float64(sh) + marginY

	// baseX := float64(island.X) * (float64(tileWidth) / 2)
	// baseY := float64(island.Y) * (float64(tileHeight) / 2)

	// denX := float64(tileWidth) / 2
	// denY := float64(tileHeight) / 2
	// corners := [][2]float64{{minWX, minWY}, {maxWX, minWY}, {minWX, maxWY}, {maxWX, maxWY}}

	// minX, minY := math.Inf(1), math.Inf(1)
	// maxX, maxY := math.Inf(-1), math.Inf(-1)
	// for _, c := range corners {
	// 	dx := (c[0] - baseX) / denX
	// 	dy := (c[1] - baseY) / denY
	// 	x := (dx + dy) / 2
	// 	y := (dy - dx) / 2
	// 	if x < minX {
	// 		minX = x
	// 	}
	// 	if x > maxX {
	// 		maxX = x
	// 	}
	// 	if y < minY {
	// 		minY = y
	// 	}
	// 	if y > maxY {
	// 		maxY = y
	// 	}
	// }

	// ix0 := int(math.Floor(minX)) - 2
	// ix1 := int(math.Ceil(maxX)) + 2
	// iy0 := int(math.Floor(minY)) - 2
	// iy1 := int(math.Ceil(maxY)) + 2

	// for y := iy0; y <= iy1; y++ {
	// 	for x := ix0; x <= ix1; x++ {
	// 		originX := (float64(x-y))*(float64(tileWidth)/2) + baseX
	// 		originY := (float64(x+y))*(float64(tileHeight)/2) + baseY

	// 		op := &ebiten.DrawImageOptions{}
	// 		op.GeoM.Translate(originX-pivotX-camera.X, originY-pivotY-camera.Y)
	// 		screen.DrawImage(seaImg, op)
	// 	}
	// }
}
