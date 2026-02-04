package systems

import (
	"cmp"
	"fmt"
	"image"
	"image/color"
	"log"
	"math"
	"slices"
	"time"

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

// FPS tracking
var (
	frameCount    int
	lastFPSUpdate time.Time
	currentFPS    float64
	renderedTiles int
	culledTiles   int
)

// Cached sea background texture - pre-rendered tiled sea for performance
var (
	cachedSeaTexture     *ebiten.Image
	cachedSeaSourceImage *ebiten.Image
	cachedSeaTileW       int
	cachedSeaTileH       int
	seaTextureGridSize   = 32 // Number of tiles in each direction for the cached texture
)

// Selection buffer for pixel-perfect building hover detection
var (
	selectionBuffer  *ebiten.Image
	displayBuffer    *ebiten.Image // Rainbow-colored version for debug display
	selectionBufferW int
	selectionBufferH int
)

// Cached queries (avoid recreating every frame)
var (
	cameraQueryCached  = donburi.NewQuery(filter.Contains(components.CameraType))
	controlQueryCached = donburi.NewQuery(filter.Contains(components.ControlType))
	worldQueryCached   = donburi.NewQuery(filter.Contains(components.WorldType))
)

// Reusable DrawImageOptions to reduce allocations
var reusableOp = &ebiten.DrawImageOptions{}
var selectionOp = &ebiten.DrawImageOptions{}

// Cache for silhouette images (building ID -> solid color silhouette)
var silhouetteCache = make(map[*ebiten.Image]*ebiten.Image)

// idToColor encodes a building ID to a rainbow color for better visibility
// Uses golden ratio to distribute colors evenly across the spectrum
func idToColor(id int) color.RGBA {
	// Use golden ratio for even color distribution
	golden := 0.618033988749895
	hue := math.Mod(float64(id)*golden, 1.0)
	return hsvToRGB(hue, 0.9, 1.0)
}

// hsvToRGB converts HSV (hue 0-1, saturation 0-1, value 0-1) to RGB
func hsvToRGB(h, s, v float64) color.RGBA {
	var r, g, b float64
	i := int(h * 6)
	f := h*6 - float64(i)
	p := v * (1 - s)
	q := v * (1 - f*s)
	t := v * (1 - (1-f)*s)

	switch i % 6 {
	case 0:
		r, g, b = v, t, p
	case 1:
		r, g, b = q, v, p
	case 2:
		r, g, b = p, v, t
	case 3:
		r, g, b = p, q, v
	case 4:
		r, g, b = t, p, v
	case 5:
		r, g, b = v, p, q
	}

	return color.RGBA{
		R: uint8(r * 255),
		G: uint8(g * 255),
		B: uint8(b * 255),
		A: 255,
	}
}

// colorToID decodes a rainbow color back to a building ID
// Since we use rainbow colors, we need to store the actual ID separately
// For now, we encode ID in the color directly using a simpler scheme
func colorToID(c color.Color) int {
	r, g, b, a := c.RGBA()
	if a == 0 {
		return 0 // transparent = no building
	}
	// RGBA returns 16-bit values, shift down to 8-bit
	return int(r>>8)<<16 | int(g>>8)<<8 | int(b>>8)
}

// idToEncodedColor encodes a building ID directly into RGB for sampling
// This is used for the actual ID lookup, separate from display color
func idToEncodedColor(id int) color.RGBA {
	return color.RGBA{
		R: uint8((id >> 16) & 0xFF),
		G: uint8((id >> 8) & 0xFF),
		B: uint8(id & 0xFF),
		A: 255,
	}
}

// getSilhouette returns a solid white silhouette of the given image (preserving alpha)
func getSilhouette(img *ebiten.Image) *ebiten.Image {
	if cached, ok := silhouetteCache[img]; ok {
		return cached
	}

	bounds := img.Bounds()
	w, h := bounds.Dx(), bounds.Dy()

	// Create a solid white image with the same alpha as the original
	whiteImg := image.NewRGBA(image.Rect(0, 0, w, h))

	// Read pixels from ebiten image
	pixels := make([]byte, w*h*4)
	img.ReadPixels(pixels)

	// Set all pixels to white, preserving alpha
	for i := 0; i < len(pixels); i += 4 {
		alpha := pixels[i+3]
		if alpha > 0 {
			whiteImg.Pix[i] = 255   // R
			whiteImg.Pix[i+1] = 255 // G
			whiteImg.Pix[i+2] = 255 // B
			whiteImg.Pix[i+3] = alpha
		}
	}

	silhouette := ebiten.NewImageFromImage(whiteImg)
	silhouetteCache[img] = silhouette
	return silhouette
}

// TileToScreen converts tile grid coordinates to isometric screen coordinates
func TileToScreen(tileX, tileY float64, tileWidth, tileHeight int) (screenX, screenY float64) {
	screenX = (tileX - tileY) * (float64(tileWidth) / 2)
	screenY = (tileX + tileY) * (float64(tileHeight) / 2)
	return
}

// ScreenToTile converts isometric screen coordinates to tile grid coordinates
func ScreenToTile(screenX, screenY float64, tileWidth, tileHeight int) (tileX, tileY float64) {
	// Inverse of the isometric transformation
	tw := float64(tileWidth) / 2
	th := float64(tileHeight) / 2
	tileX = (screenX/tw + screenY/th) / 2
	tileY = (screenY/th - screenX/tw) / 2
	return
}

var rendererQuery = donburi.NewQuery(filter.Contains(components.IslandType))

type RenderableTile struct {
	isoX, isoY     float64
	originX        float64
	originY        float64
	bottomY        float64
	topX, topY     int
	Image          *ebiten.Image
	Layer          int
	pivotX         float64
	pivotY         float64
	SpriteRotation int   // 0-3: number of 90° clockwise rotations to apply
	sortKey        int64 // Pre-computed sort key: layer << 32 | topY << 16 | topX
	BuildingID     int   // Building ID for selection buffer (0 = not selectable)
	IsSelectable   bool  // True if this tile should appear in selection buffer (buildings, roads, plazas)
}

func RenderSystem(world donburi.World, screen *ebiten.Image, grid bool, currentRotation rotation.Rotation) {
	// Update FPS counter
	frameCount++
	now := time.Now()
	if now.Sub(lastFPSUpdate) >= time.Second {
		currentFPS = float64(frameCount) / now.Sub(lastFPSUpdate).Seconds()
		frameCount = 0
		lastFPSUpdate = now
	}

	tileWidth := zoom.TileSize()
	tileHeight := zoom.TileHeight()

	// Pre-allocate with estimated capacity to reduce allocations
	renderableTiles := make([]RenderableTile, 0, 10000)

	var camera *components.Camera
	var overlay bool
	var worldWidth, worldHeight int = 500, 500 // Default world size
	cameraQueryCached.Each(world, func(entry *donburi.Entry) {
		camera = components.CameraType.Get(entry)
	})
	controlQueryCached.Each(world, func(entry *donburi.Entry) {
		overlay = components.ControlType.Get(entry).OverlayVisible
	})
	worldQueryCached.Each(world, func(entry *donburi.Entry) {
		worldComp := components.WorldType.Get(entry)
		if worldComp != nil {
			worldWidth = worldComp.Width
			worldHeight = worldComp.Height
		}
	})
	if camera == nil {
		log.Println("No camera entity found")
		return
	}

	// Get zoom level (default to 1.0 if not set)
	zoomLevel := camera.Zoom
	if zoomLevel <= 0 {
		zoomLevel = 1.0
	}

	// Get screen dimensions for zoom centering
	screenW, screenH := screen.Bounds().Dx(), screen.Bounds().Dy()
	screenCenterX := float64(screenW) / 2
	screenCenterY := float64(screenH) / 2

	// Convert camera position from tile coordinates to screen coordinates
	cameraScreenX, cameraScreenY := TileToScreen(camera.X, camera.Y, tileWidth, tileHeight)

	// Calculate visible bounds in world coordinates for frustum culling.
	// The zoom is centered on screen center, so we need to calculate what world
	// coordinates are visible at the screen edges.
	// Screen position (sx, sy) maps to world-relative position:
	//   relX = (sx - screenCenterX * (1 - zoom)) / zoom
	// Then world position = relX + cameraScreenX
	// Use a margin that's larger at high zoom (where sprites are bigger on screen).
	zoomOffset := 1 - zoomLevel
	margin := 500.0 + 1000.0*zoomLevel // 500 base + 1000 extra at zoom 1.0 = 1500 at full zoom
	visMinX := (0-screenCenterX*zoomOffset)/zoomLevel + cameraScreenX - margin
	visMinY := (0-screenCenterY*zoomOffset)/zoomLevel + cameraScreenY - margin
	visMaxX := (float64(screenW)-screenCenterX*zoomOffset)/zoomLevel + cameraScreenX + margin
	visMaxY := (float64(screenH)-screenCenterY*zoomOffset)/zoomLevel + cameraScreenY + margin

	// Reset tile counters
	culledTiles = 0
	localRendered := 0

	// Draw sea background once (before islands) - find a sea tile from any island
	var seaImg *ebiten.Image
	var seaFound bool
	rendererQuery.Each(world, func(entry *donburi.Entry) {
		if seaFound {
			return
		}
		island := components.IslandType.Get(entry)
		for _, tileEntry := range island.Tiles[buildings.KindSeaID] {
			tile := components.TileType.Get(tileEntry)
			if tile.Image == nil {
				continue
			}
			b := components.BuildingType.Get(tileEntry)
			if b != nil && b.BuildingID == 1201 { // deep sea tile
				seaImg = tile.Image
				seaFound = true
				return
			}
			if seaImg == nil {
				seaImg = tile.Image
			}
		}
	})
	if seaImg != nil {
		drawSeaBackground(screen, cameraScreenX, cameraScreenY, seaImg, tileWidth, tileHeight, zoomLevel, screenCenterX, screenCenterY)
	}

	rendererQuery.Each(world, func(entry *donburi.Entry) {
		island := components.IslandType.Get(entry)

		// Rotate island origin around world origin using the local origin corner
		// that maps to (0,0) after RotatePosition.
		cornerX, cornerY := 0, 0
		switch currentRotation {
		case rotation.DEG90:
			cornerY = island.Height - 1
		case rotation.DEG180:
			cornerX = island.Width - 1
			cornerY = island.Height - 1
		case rotation.DEG270:
			cornerX = island.Width - 1
		}
		cornerWorldX := island.X + float64(cornerX)
		cornerWorldY := island.Y + float64(cornerY)
		rotatedIslandX, rotatedIslandY := rotation.RotateWorldPosition(
			cornerWorldX, cornerWorldY, worldWidth, worldHeight, currentRotation)

		// Swap island dimensions for 90° and 270° rotations
		islandW, islandH := island.Width, island.Height
		if currentRotation == rotation.DEG90 || currentRotation == rotation.DEG270 {
			islandW, islandH = island.Height, island.Width
		}

		// Quick island-level frustum culling (use rotated position)
		islandIsoX := (rotatedIslandX - rotatedIslandY) * (float64(tileWidth) / 2)
		islandIsoY := (rotatedIslandX + rotatedIslandY) * (float64(tileHeight) / 2)
		islandMaxX := islandIsoX + float64(islandW+islandH)*(float64(tileWidth)/2)
		islandMaxY := islandIsoY + float64(islandW+islandH)*(float64(tileHeight)/2)

		// Skip entire island if completely outside view
		if islandMaxX < visMinX || islandIsoX > visMaxX || islandMaxY < visMinY || islandIsoY > visMaxY {
			return
		}

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

		// Include Sea layer for Surf/Estuary coast transitions (deep sea 1201 is filtered below)
		layerOrder := []string{
			buildings.KindSeaID,                 // Surf, Estuary - deep sea filtered out below
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
			isSelectable := layerID == buildings.KindBuildingsID || layerID == buildings.KindRoadsID
			for _, tileEntry := range island.Tiles[layerID] {
				pos := components.PositionType.Get(tileEntry)
				tile := components.TileType.Get(tileEntry)
				bld := components.BuildingType.Get(tileEntry)

				// Skip deep sea tiles (1201) - these are drawn by the sea background
				if layerID == buildings.KindSeaID {
					if bld != nil && bld.BuildingID == 1201 {
						continue
					}
				}

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
				// Use original island dimensions for the rotation calculation
				lx := int(pos.X - island.X)
				ly := int(pos.Y - island.Y)
				rxl, ryl := rotation.RotatePosition(lx, ly, island.Width, island.Height, currentRotation)

				// Tile origin convention: (x,y) maps to the top point of the isometric diamond.
				// Then compute local tile position in isometric and add to rotated island offset
				localX := float64(rxl)
				localY := float64(ryl)
				originX := (localX-localY)*(float64(tileWidth)/2) + islandIsoX
				originY := (localX+localY)*(float64(tileHeight)/2) + islandIsoY
				originY -= pos.Offset

				// Calculate rotated world position for sorting
				rotatedX := rxl + int(rotatedIslandX)
				rotatedY := ryl + int(rotatedIslandY)

				// Tile-level frustum culling - visible bounds already include margin
				if originX < visMinX || originX > visMaxX || originY < visMinY || originY > visMaxY {
					culledTiles++
					continue
				}

				localRendered++
				drawX := originX - float64(tile.PivotX)
				drawY := originY - float64(tile.PivotY)

				bottomY := drawY + float64(tile.Image.Bounds().Dy())
				layer := layerPriority[layerID]
				// Pre-compute sort key: layer in high bits, then topY, then topX
				// This allows single integer comparison instead of multiple comparisons
				sortKey := int64(layer)<<32 | int64(rotatedY+10000)<<16 | int64(rotatedX+10000)

				buildingID := 0
				if isSelectable && bld != nil {
					buildingID = bld.BuildingID
				}

				renderableTiles = append(renderableTiles, RenderableTile{
					isoX:           drawX,
					isoY:           drawY,
					originX:        originX,
					originY:        originY,
					bottomY:        bottomY,
					topX:           rotatedX,
					topY:           rotatedY,
					Image:          tile.Image,
					Layer:          layer,
					pivotX:         float64(tile.PivotX),
					pivotY:         float64(tile.PivotY),
					SpriteRotation: tile.SpriteRotation,
					sortKey:        sortKey,
					BuildingID:     buildingID,
					IsSelectable:   isSelectable,
				})
			}
		}
	})

	renderedTiles = localRendered

	// Sort by pre-computed key for faster sorting using slices.SortFunc
	slices.SortFunc(renderableTiles, func(a, b RenderableTile) int {
		return cmp.Compare(a.sortKey, b.sortKey)
	})

	for _, tile := range renderableTiles {
		if tile.Image == nil {
			continue
		}
		// Reuse the DrawImageOptions to reduce allocations
		reusableOp.GeoM.Reset()
		reusableOp.ColorScale.Reset()

		// Apply sprite rotation for tiles with Rotate=0 in COD but non-zero orientation
		if tile.SpriteRotation > 0 {
			w, h := float64(tile.Image.Bounds().Dx()), float64(tile.Image.Bounds().Dy())
			// Rotate around image center
			reusableOp.GeoM.Translate(-w/2, -h/2)
			reusableOp.GeoM.Rotate(float64(tile.SpriteRotation) * math.Pi / 2)
			reusableOp.GeoM.Translate(w/2, h/2)
		}

		// Calculate position relative to camera
		relX := tile.isoX - cameraScreenX
		relY := tile.isoY - cameraScreenY

		// Apply zoom: scale the image and position around screen center
		reusableOp.GeoM.Scale(zoomLevel, zoomLevel)
		reusableOp.GeoM.Translate(relX*zoomLevel+screenCenterX*(1-zoomLevel), relY*zoomLevel+screenCenterY*(1-zoomLevel))
		screen.DrawImage(tile.Image, reusableOp)

		if overlay {
			// Apply zoom transformation to overlay coordinates
			relOX := tile.originX - cameraScreenX
			relOY := tile.originY - cameraScreenY
			x := int(relOX*zoomLevel + screenCenterX*(1-zoomLevel))
			y := int(relOY*zoomLevel + screenCenterY*(1-zoomLevel))
			text.Draw(screen, "+", basicfont.Face7x13, x-3, y+5, color.RGBA{255, 255, 255, 255})
			text.Draw(screen, fmt.Sprintf("%d", tile.Layer), basicfont.Face7x13, x+6, y+5, color.RGBA{255, 255, 0, 255})
		}
	}

	// Check if we should show selection buffer debug view
	var showSelectionBuffer bool
	controlQueryCached.Each(world, func(entry *donburi.Entry) {
		ctrl := components.ControlType.Get(entry)
		showSelectionBuffer = ctrl.SelectionBufferView
	})

	// Render selection buffer for pixel-perfect building hover detection
	renderSelectionBuffer(renderableTiles, screenW, screenH, cameraScreenX, cameraScreenY, zoomLevel, screenCenterX, screenCenterY, showSelectionBuffer)

	// Sample selection buffer at mouse position to get hovered building ID
	sampleSelectionBuffer(world)

	// Optionally show rainbow-colored display buffer instead of normal rendering (toggle with 'B' key)
	if showSelectionBuffer && displayBuffer != nil {
		screen.Clear()
		screen.DrawImage(displayBuffer, nil)
	}

	if grid {
		renderDebugGrid(world, screen, float64(tileWidth), float64(tileHeight))
	}

	// Draw HUD overlay with camera position and hovered island info
	renderHUDOverlay(world, screen, camera, currentRotation)
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
				// Convert island world position to isometric screen coordinates
				islandIsoX := (island.X - island.Y) * (tileWidth / 2)
				islandIsoY := (island.X + island.Y) * (tileHeight / 2)
				originX := (float64(lx)-float64(ly))*(tileWidth/2) + islandIsoX
				originY := (float64(lx)+float64(ly))*(tileHeight/2) + islandIsoY
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

// ensureSeaTextureCache creates or updates the cached sea texture if needed
func ensureSeaTextureCache(seaImg *ebiten.Image, tileWidth, tileHeight int) {
	if seaImg == nil {
		return
	}

	// Check if we need to rebuild the cache
	if cachedSeaTexture != nil && cachedSeaSourceImage == seaImg &&
		cachedSeaTileW == tileWidth && cachedSeaTileH == tileHeight {
		return // Cache is still valid
	}

	// Build a large tiled sea texture
	// The sea tile is an isometric diamond, so we need to tile it properly
	imgW := seaImg.Bounds().Dx()
	imgH := seaImg.Bounds().Dy()

	// Create a texture large enough to cover the visible area when tiled
	// We use a grid of tiles that will be repeated
	textureW := seaTextureGridSize * tileWidth
	textureH := seaTextureGridSize * tileHeight

	cachedSeaTexture = ebiten.NewImage(textureW, textureH)

	// Draw tiles in an isometric pattern to fully cover the rectangular texture.
	// We need to iterate over screen-space rows and place tiles to fill each row.
	// For each screen Y, we need tiles that cover from X=0 to X=textureW.
	// The isometric tile origin is at the top point of the diamond.
	// Tile at grid (gx, gy) has screen origin at:
	//   screenX = (gx - gy) * tileWidth/2
	//   screenY = (gx + gy) * tileHeight/2
	// We need to cover screenY from 0 to textureH and screenX from 0 to textureW.

	// Calculate how many tiles we need to cover the texture
	// Each tile covers tileHeight in Y direction, but due to isometric layout
	// we need extra tiles to fill the rectangular area
	tilesNeeded := seaTextureGridSize * 3

	for gy := -tilesNeeded; gy < tilesNeeded; gy++ {
		for gx := -tilesNeeded; gx < tilesNeeded; gx++ {
			// Isometric screen position for this grid cell
			screenX := float64(gx-gy) * float64(tileWidth) / 2
			screenY := float64(gx+gy) * float64(tileHeight) / 2

			// The sea sprite's anchor is at the top point of the diamond
			// Draw position needs to account for the sprite dimensions
			drawX := screenX - float64(imgW)/2 + float64(tileWidth)/2
			drawY := screenY

			// Skip if completely outside texture bounds
			if drawX+float64(imgW) < 0 || drawX > float64(textureW) ||
				drawY+float64(imgH) < 0 || drawY > float64(textureH) {
				continue
			}

			op := &ebiten.DrawImageOptions{}
			op.GeoM.Translate(drawX, drawY)
			cachedSeaTexture.DrawImage(seaImg, op)
		}
	}

	cachedSeaSourceImage = seaImg
	cachedSeaTileW = tileWidth
	cachedSeaTileH = tileHeight
}

func drawSeaBackground(screen *ebiten.Image, cameraScreenX, cameraScreenY float64, seaImg *ebiten.Image, tileWidth, tileHeight int, zoomLevel, screenCenterX, screenCenterY float64) {
	if seaImg == nil {
		return
	}

	// Ensure we have a cached sea texture
	ensureSeaTextureCache(seaImg, tileWidth, tileHeight)
	if cachedSeaTexture == nil {
		return
	}

	sw, sh := screen.Bounds().Dx(), screen.Bounds().Dy()
	textureW := float64(cachedSeaTexture.Bounds().Dx())
	textureH := float64(cachedSeaTexture.Bounds().Dy())

	// Calculate the visible world-space bounds accounting for zoom centered on screen center.
	// Screen position (sx, sy) maps to world-relative position:
	//   relX = (sx - screenCenterX * (1 - zoom)) / zoom
	//   relY = (sy - screenCenterY * (1 - zoom)) / zoom
	zoomOffset := 1 - zoomLevel
	minRelX := (0 - screenCenterX*zoomOffset) / zoomLevel
	minRelY := (0 - screenCenterY*zoomOffset) / zoomLevel
	maxRelX := (float64(sw) - screenCenterX*zoomOffset) / zoomLevel
	maxRelY := (float64(sh) - screenCenterY*zoomOffset) / zoomLevel

	// Calculate the offset based on camera position
	// The texture repeats, so we use modulo to find the offset within one texture tile
	offsetX := math.Mod(cameraScreenX, textureW)
	if offsetX < 0 {
		offsetX += textureW
	}
	offsetY := math.Mod(cameraScreenY, textureH)
	if offsetY < 0 {
		offsetY += textureH
	}

	// Calculate tile range needed to cover the visible world area
	startTx := int(math.Floor((minRelX + offsetX) / textureW))
	startTy := int(math.Floor((minRelY + offsetY) / textureH))
	endTx := int(math.Ceil((maxRelX + offsetX) / textureW))
	endTy := int(math.Ceil((maxRelY + offsetY) / textureH))

	// Draw the sea texture tiles - reuse DrawImageOptions to reduce allocations
	seaOp := &ebiten.DrawImageOptions{}
	for ty := startTy - 1; ty <= endTy+1; ty++ {
		for tx := startTx - 1; tx <= endTx+1; tx++ {
			// Position of this texture tile in world coordinates (relative to camera)
			relX := float64(tx)*textureW - offsetX
			relY := float64(ty)*textureH - offsetY

			// Apply zoom transformation centered on screen
			seaOp.GeoM.Reset()
			seaOp.GeoM.Scale(zoomLevel, zoomLevel)
			seaOp.GeoM.Translate(
				relX*zoomLevel+screenCenterX*zoomOffset,
				relY*zoomLevel+screenCenterY*zoomOffset,
			)
			screen.DrawImage(cachedSeaTexture, seaOp)
		}
	}
}

// renderHUDOverlay draws the camera position and hovered island information in the upper left
func renderHUDOverlay(world donburi.World, screen *ebiten.Image, camera *components.Camera, currentRotation rotation.Rotation) {
	if camera == nil {
		return
	}

	face := basicfont.Face7x13
	textColor := color.RGBA{255, 255, 255, 255}
	highlightColor := color.RGBA{255, 255, 0, 255}

	// Get control state for hovered island info
	var ctrl *components.Control
	controlQueryCached.Each(world, func(entry *donburi.Entry) {
		ctrl = components.ControlType.Get(entry)
	})

	y := 15
	lineHeight := 15

	// FPS counter
	fpsColor := color.RGBA{0, 255, 0, 255}
	if currentFPS < 30 {
		fpsColor = color.RGBA{255, 0, 0, 255}
	} else if currentFPS < 55 {
		fpsColor = color.RGBA{255, 255, 0, 255}
	}
	text.Draw(screen, fmt.Sprintf("FPS: %.1f", currentFPS), face, 10, y, fpsColor)
	y += lineHeight

	// Tile render stats
	text.Draw(screen, fmt.Sprintf("Tiles: %d drawn, %d culled", renderedTiles, culledTiles), face, 10, y, textColor)
	y += lineHeight

	// Camera position (in tile coordinates)
	text.Draw(screen, fmt.Sprintf("Camera: (%.1f, %.1f)", camera.X, camera.Y), face, 10, y, textColor)
	y += lineHeight

	// Zoom level
	text.Draw(screen, fmt.Sprintf("Zoom: %.0f%%", camera.Zoom*100), face, 10, y, textColor)
	y += lineHeight

	// Current rotation
	text.Draw(screen, fmt.Sprintf("Rotation: %s", currentRotation.String()), face, 10, y, textColor)
	y += lineHeight

	if ctrl != nil {
		// Mouse tile position
		text.Draw(screen, fmt.Sprintf("Mouse Tile: (%.1f, %.1f)", ctrl.MouseTileX, ctrl.MouseTileY), face, 10, y, textColor)
		y += lineHeight

		// Hovered island
		if ctrl.HoveredIsland >= 0 {
			text.Draw(screen, fmt.Sprintf("Island: %d", ctrl.HoveredIsland), face, 10, y, highlightColor)
		} else {
			text.Draw(screen, "Island: -", face, 10, y, textColor)
		}
		y += lineHeight

		// Hovered building
		if ctrl.HoveredBuildingID > 0 {
			text.Draw(screen, fmt.Sprintf("Building: %d", ctrl.HoveredBuildingID), face, 10, y, highlightColor)
		} else {
			text.Draw(screen, "Building: -", face, 10, y, textColor)
		}
	}
}

// renderSelectionBuffer draws buildings to an offscreen buffer using ID-encoded colors
// Also creates a display buffer with rainbow colors for debug visualization
func renderSelectionBuffer(tiles []RenderableTile, screenW, screenH int, cameraScreenX, cameraScreenY, zoomLevel, screenCenterX, screenCenterY float64, showDisplay bool) {
	// Create or resize selection buffer if needed
	if selectionBuffer == nil || selectionBufferW != screenW || selectionBufferH != screenH {
		selectionBuffer = ebiten.NewImage(screenW, screenH)
		displayBuffer = ebiten.NewImage(screenW, screenH)
		selectionBufferW = screenW
		selectionBufferH = screenH
	}

	// Clear buffers
	selectionBuffer.Clear()
	if showDisplay {
		displayBuffer.Clear()
	}

	// Draw selectable tiles (buildings, roads, plazas) with their ID-encoded color as solid silhouettes
	for _, tile := range tiles {
		if !tile.IsSelectable || tile.BuildingID == 0 || tile.Image == nil {
			continue
		}

		// Get or create silhouette (solid white version of the sprite)
		silhouette := getSilhouette(tile.Image)

		selectionOp.GeoM.Reset()
		selectionOp.ColorScale.Reset()

		// Apply sprite rotation if needed
		if tile.SpriteRotation > 0 {
			w, h := float64(tile.Image.Bounds().Dx()), float64(tile.Image.Bounds().Dy())
			selectionOp.GeoM.Translate(-w/2, -h/2)
			selectionOp.GeoM.Rotate(float64(tile.SpriteRotation) * math.Pi / 2)
			selectionOp.GeoM.Translate(w/2, h/2)
		}

		// Calculate position relative to camera (same as main render)
		relX := tile.isoX - cameraScreenX
		relY := tile.isoY - cameraScreenY

		// Apply zoom transformation
		selectionOp.GeoM.Scale(zoomLevel, zoomLevel)
		selectionOp.GeoM.Translate(relX*zoomLevel+screenCenterX*(1-zoomLevel), relY*zoomLevel+screenCenterY*(1-zoomLevel))

		// Use encoded color for ID lookup (direct RGB encoding)
		idColor := idToEncodedColor(tile.BuildingID)
		selectionOp.ColorScale.ScaleWithColor(idColor)
		selectionBuffer.DrawImage(silhouette, selectionOp)

		// Also draw to display buffer with rainbow colors if showing debug view
		if showDisplay {
			selectionOp.ColorScale.Reset()
			rainbowColor := idToColor(tile.BuildingID)
			selectionOp.ColorScale.ScaleWithColor(rainbowColor)
			displayBuffer.DrawImage(silhouette, selectionOp)
		}
	}
}

// sampleSelectionBuffer reads the pixel at mouse position and decodes the building ID
func sampleSelectionBuffer(world donburi.World) {
	if selectionBuffer == nil {
		return
	}

	mouseX, mouseY := ebiten.CursorPosition()

	// Bounds check
	if mouseX < 0 || mouseY < 0 || mouseX >= selectionBufferW || mouseY >= selectionBufferH {
		return
	}

	// Sample the pixel color at mouse position
	pixelColor := selectionBuffer.At(mouseX, mouseY)
	buildingID := colorToID(pixelColor)

	// Update control component
	controlQueryCached.Each(world, func(entry *donburi.Entry) {
		ctrl := components.ControlType.Get(entry)
		ctrl.HoveredBuildingID = buildingID
	})
}
