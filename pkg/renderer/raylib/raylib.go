//go:build raylib

package raylibrenderer

import (
	"errors"
	"fmt"
	"image"
	"image/color"
	"log"
	"math"
	"os"
	"path/filepath"
	"strings"

	rl "github.com/gen2brain/raylib-go/raylib"
	"github.com/hajimehoshi/ebiten/v2"
	"github.com/siredmar/mdcii-engine/pkg/cod/buildings"
	"github.com/siredmar/mdcii-engine/pkg/ecs/components"
	"github.com/siredmar/mdcii-engine/pkg/ecs/world"
	"github.com/siredmar/mdcii-engine/pkg/renderer"
	"github.com/siredmar/mdcii-engine/pkg/texture/atlas"
	"github.com/siredmar/mdcii-engine/pkg/world/rotation"
	"github.com/siredmar/mdcii-engine/pkg/world/zoom"
	"github.com/yohamta/donburi"
	"github.com/yohamta/donburi/filter"
)

type Screen struct {
	width  int
	height int
}

func NewScreen(width, height int) Screen {
	return Screen{width: width, height: height}
}

func (s Screen) Width() int {
	return s.width
}

func (s Screen) Height() int {
	return s.height
}

func (s Screen) Clear() {
	rl.ClearBackground(rl.Black)
}

type Renderer struct {
	textures        map[*ebiten.Image]rl.Texture2D
	atlasTextures   []rl.Texture2D
	currentAtlas    int
	currentAtlasKey string
	atlasImages     []*image.RGBA
	atlasVersion    int
	atlasPath       string
	imageCache      map[*ebiten.Image]*image.RGBA
	failedImages    map[*ebiten.Image]struct{}
	invisibleImages map[*ebiten.Image]struct{}
	camera          rl.Camera3D
	ppu             float32
	initialized     bool
	selectionMode   bool
	screenWidth     int
	screenHeight    int

	selectionRT     rl.RenderTexture2D
	displayRT       rl.RenderTexture2D
	selectionRTW    int
	selectionRTH    int
	silhouetteCache map[int]rl.Texture2D // atlasIndex -> white silhouette texture
}

func New() *Renderer {
	return &Renderer{
		textures:        make(map[*ebiten.Image]rl.Texture2D),
		imageCache:      make(map[*ebiten.Image]*image.RGBA),
		failedImages:    make(map[*ebiten.Image]struct{}),
		invisibleImages: make(map[*ebiten.Image]struct{}),
		silhouetteCache: make(map[int]rl.Texture2D),
	}
}

func (r *Renderer) Render(w *world.World, screen renderer.Screen, grid bool, currentRotation rotation.Rotation) {
	raylibScreen, ok := screen.(Screen)
	if !ok {
		return
	}

	if !r.initialized {
		// Use resizable window to prevent automatic letterboxing/centering
		rl.SetConfigFlags(rl.FlagWindowResizable | rl.FlagVsyncHint)
		rl.InitWindow(int32(raylibScreen.width), int32(raylibScreen.height), "mdcii-raylib")
		rl.SetWindowSize(raylibScreen.width, raylibScreen.height)
		rl.SetTraceLogLevel(rl.LogError)
		rl.SetTargetFPS(60)
		r.camera = rl.Camera3D{
			Position:   rl.NewVector3(0, 20, 20),
			Target:     rl.NewVector3(0, 0, 0),
			Up:         rl.NewVector3(0, 1, 0),
			Fovy:       45,
			Projection: rl.CameraPerspective,
		}
		r.screenWidth = raylibScreen.width
		r.screenHeight = raylibScreen.height
		r.initialized = true
	}

	if rl.WindowShouldClose() {
		return
	}

	var camera *components.Camera
	var ecsWorld *components.World
	cameraQueryCached := donburi.NewQuery(filter.Contains(components.CameraType))
	cameraQueryCached.Each(w.World, func(entry *donburi.Entry) {
		camera = components.CameraType.Get(entry)
	})
	worldQueryCached := donburi.NewQuery(filter.Contains(components.WorldType))
	worldQueryCached.Each(w.World, func(entry *donburi.Entry) {
		ecsWorld = components.WorldType.Get(entry)
	})
	if camera == nil {
		return
	}

	zoomLevel := camera.Zoom
	if zoomLevel <= 0 {
		zoomLevel = 1.0
	}
	r.ensureIsoCamera(raylibScreen, zoomLevel, camera)

	r.ensureAtlasTextures()

	// Get actual framebuffer dimensions (may differ from requested due to RGFW behavior)
	fbWidth := int32(rl.GetRenderWidth())
	fbHeight := int32(rl.GetRenderHeight())
	if fbWidth <= 0 {
		fbWidth = int32(raylibScreen.width)
	}
	if fbHeight <= 0 {
		fbHeight = int32(raylibScreen.height)
	}

	// Use actual framebuffer dimensions for rendering calculations
	actualWidth := int(fbWidth)
	actualHeight := int(fbHeight)

	tileWidth := zoom.TileSize()
	tileHeight := zoom.TileHeight()

	// Handle deferred island centering (now that we know actual screen size)
	if camera.CenterOnIsland > 0 && ecsWorld != nil {
		islandIdx := camera.CenterOnIsland - 1
		if islandIdx < len(ecsWorld.Islands) {
			island := components.IslandType.Get(ecsWorld.Islands[islandIdx])
			// Compute screen center offset in tile coordinates for actual screen size
			screenCenterX := float64(actualWidth) / 2
			screenCenterY := float64(actualHeight) / 2
			screenCenterTileX, screenCenterTileY := ScreenToTile(screenCenterX, screenCenterY, tileWidth, tileHeight)
			camera.X = island.X + float64(island.Width)/2 - screenCenterTileX
			camera.Y = island.Y + float64(island.Height)/2 - screenCenterTileY
			log.Printf("Debug: centered camera on island %d at (%.1f, %.1f) for screen %dx%d\n",
				islandIdx, camera.X, camera.Y, actualWidth, actualHeight)
		}
		camera.CenterOnIsland = 0 // Only do this once
	}

	rl.BeginDrawing()
	// Reset viewport to full framebuffer to override raylib's letterboxing
	rl.Viewport(0, 0, fbWidth, fbHeight)
	rl.ClearBackground(rl.Black)

	camScreenX, camScreenY := TileToScreen(camera.X, camera.Y, tileWidth, tileHeight)
	r.drawSeaBackground(w.World, actualWidth, actualHeight, camera, zoomLevel, tileWidth, tileHeight, camScreenX, camScreenY)

	var ctrl *components.Control
	controlQueryCached := donburi.NewQuery(filter.Contains(components.ControlType))
	controlQueryCached.Each(w.World, func(entry *donburi.Entry) {
		ctrl = components.ControlType.Get(entry)
	})
	showSelectionBuffer := ctrl != nil && ctrl.SelectionBufferView

	r.ensureSelectionRT(int32(actualWidth), int32(actualHeight))

	renderedTilesRaylib := 0
	forEachTile(w.World, currentRotation, func(tile renderTile) {
		relX := tile.isoX - camScreenX
		relY := tile.isoY - camScreenY

		if tile.srcW <= 0 || tile.srcH <= 0 {
			return
		}
		if tile.atlasIndex < 0 || tile.atlasIndex >= len(r.atlasTextures) {
			return
		}
		texture := r.atlasTextures[tile.atlasIndex]
		if !rl.IsTextureValid(texture) {
			return
		}

		src := rl.NewRectangle(float32(tile.srcX), float32(tile.srcY), float32(tile.srcW), float32(tile.srcH))
		src = insetRect(src, 0.5)
		if src.Width <= 0 || src.Height <= 0 {
			return
		}

		screenX := float32(relX*zoomLevel + float64(actualWidth)/2*(1-zoomLevel))
		screenY := float32(relY*zoomLevel + float64(actualHeight)/2*(1-zoomLevel))
		zoomScale := float32(zoomLevel)
		size := rl.NewVector2(src.Width*zoomScale, src.Height*zoomScale)
		origin := rl.Vector2Zero()

		rl.DrawTexturePro(
			texture,
			src,
			rl.NewRectangle(screenX, screenY, size.X, size.Y),
			origin,
			0,
			rl.White,
		)
		renderedTilesRaylib++
	})

	if showSelectionBuffer {
		r.renderDisplayBuffer(w.World, currentRotation, camScreenX, camScreenY, zoomLevel, actualWidth, actualHeight)
	}

	r.renderHUDOverlay(w.World, camera, currentRotation, renderedTilesRaylib)

	rl.EndDrawing()

	r.renderSelectionBufferOffscreen(w.World, currentRotation, camScreenX, camScreenY, zoomLevel, actualWidth, actualHeight)
}

type seaTileMeta struct {
	atlasIndex int
	srcX       int
	srcY       int
	srcW       int
	srcH       int
	pivotX     int
	pivotY     int
}

func findSeaTile(world donburi.World) (seaTileMeta, bool) {
	var sea seaTileMeta
	found := false
	query := donburi.NewQuery(filter.Contains(components.IslandType))
	query.Each(world, func(entry *donburi.Entry) {
		if found {
			return
		}
		island := components.IslandType.Get(entry)
		for _, tileEntry := range island.Tiles[buildings.KindSeaID] {
			tile := components.TileType.Get(tileEntry)
			if tile == nil || tile.Image == nil {
				continue
			}
			if tile.SrcW <= 0 || tile.SrcH <= 0 {
				continue
			}
			sea = seaTileMeta{
				atlasIndex: tile.AtlasIndex,
				srcX:       tile.SrcX,
				srcY:       tile.SrcY,
				srcW:       tile.SrcW,
				srcH:       tile.SrcH,
				pivotX:     tile.PivotX,
				pivotY:     tile.PivotY,
			}
			found = true
			return
		}
	})
	return sea, found
}

func (r *Renderer) drawSeaBackground(world donburi.World, screenWidth, screenHeight int, camera *components.Camera, zoomLevel float64, tileWidth, tileHeight int, camScreenX, camScreenY float64) {
	sea, ok := findSeaTile(world)
	if !ok {
		return
	}
	if sea.atlasIndex < 0 || sea.atlasIndex >= len(r.atlasTextures) {
		return
	}
	texture := r.atlasTextures[sea.atlasIndex]
	if !rl.IsTextureValid(texture) {
		return
	}

	src := rl.NewRectangle(float32(sea.srcX), float32(sea.srcY), float32(sea.srcW), float32(sea.srcH))
	src = insetRect(src, 0.5)
	if src.Width <= 0 || src.Height <= 0 {
		return
	}

	zoomScale := float32(zoomLevel)
	size := rl.NewVector2(src.Width*zoomScale, src.Height*zoomScale)
	camTileX := int(math.Floor(camera.X))
	camTileY := int(math.Floor(camera.Y))
	// Isometric projection means screen corners map to tiles far from center.
	// Compute the tile coordinates of all four screen corners and use the
	// maximum extent to ensure full coverage.
	screenCenterX := float64(screenWidth) / 2
	screenCenterY := float64(screenHeight) / 2
	corners := [4][2]float64{
		{0, 0},
		{float64(screenWidth), 0},
		{0, float64(screenHeight)},
		{float64(screenWidth), float64(screenHeight)},
	}
	maxRange := 0
	for _, c := range corners {
		relX := (c[0] - screenCenterX*(1-zoomLevel)) / zoomLevel
		relY := (c[1] - screenCenterY*(1-zoomLevel)) / zoomLevel
		worldX := relX + camScreenX
		worldY := relY + camScreenY
		tx, ty := ScreenToTile(worldX, worldY, tileWidth, tileHeight)
		dx := int(math.Ceil(math.Abs(tx - camera.X)))
		dy := int(math.Ceil(math.Abs(ty - camera.Y)))
		if dx > maxRange {
			maxRange = dx
		}
		if dy > maxRange {
			maxRange = dy
		}
	}
	maxRange += 4
	rangeX := maxRange
	rangeY := maxRange

	for gx := camTileX - rangeX; gx <= camTileX+rangeX; gx++ {
		for gy := camTileY - rangeY; gy <= camTileY+rangeY; gy++ {
			isoX, isoY := TileToScreen(float64(gx), float64(gy), tileWidth, tileHeight)
			drawX := isoX - float64(src.Width)/2 + float64(tileWidth)/2
			drawY := isoY
			relX := drawX - camScreenX
			relY := drawY - camScreenY
			screenX := float32(relX*zoomLevel + float64(screenWidth)/2*(1-zoomLevel))
			screenY := float32(relY*zoomLevel + float64(screenHeight)/2*(1-zoomLevel))

			rl.DrawTexturePro(
				texture,
				src,
				rl.NewRectangle(screenX, screenY, size.X, size.Y),
				rl.Vector2Zero(),
				0,
				rl.White,
			)
		}
	}
}

func idToEncodedColor(id int) rl.Color {
	return rl.NewColor(
		uint8((id>>16)&0xFF),
		uint8((id>>8)&0xFF),
		uint8(id&0xFF),
		255,
	)
}

func colorToID(c color.RGBA) int {
	if c.A == 0 {
		return 0
	}
	return int(c.R)<<16 | int(c.G)<<8 | int(c.B)
}

func idToRainbowColor(id int) rl.Color {
	golden := 0.618033988749895
	hue := math.Mod(float64(id)*golden, 1.0)
	r, g, b := hsvToRGBValues(hue, 0.9, 1.0)
	return rl.NewColor(uint8(r*255), uint8(g*255), uint8(b*255), 255)
}

func hsvToRGBValues(h, s, v float64) (float64, float64, float64) {
	i := int(h * 6)
	f := h*6 - float64(i)
	p := v * (1 - s)
	q := v * (1 - f*s)
	t := v * (1 - (1-f)*s)
	switch i % 6 {
	case 0:
		return v, t, p
	case 1:
		return q, v, p
	case 2:
		return p, v, t
	case 3:
		return p, q, v
	case 4:
		return t, p, v
	case 5:
		return v, p, q
	}
	return 0, 0, 0
}

func (r *Renderer) ensureSelectionRT(w, h int32) {
	if r.selectionRTW == int(w) && r.selectionRTH == int(h) && rl.IsRenderTextureValid(r.selectionRT) {
		return
	}
	if rl.IsRenderTextureValid(r.selectionRT) {
		rl.UnloadRenderTexture(r.selectionRT)
	}
	if rl.IsRenderTextureValid(r.displayRT) {
		rl.UnloadRenderTexture(r.displayRT)
	}
	r.selectionRT = rl.LoadRenderTexture(w, h)
	r.displayRT = rl.LoadRenderTexture(w, h)
	r.selectionRTW = int(w)
	r.selectionRTH = int(h)
}

func (r *Renderer) getSilhouetteTexture(atlasIndex int) rl.Texture2D {
	if tex, ok := r.silhouetteCache[atlasIndex]; ok {
		return tex
	}
	if atlasIndex < 0 || atlasIndex >= len(r.atlasTextures) {
		return rl.Texture2D{}
	}
	srcTex := r.atlasTextures[atlasIndex]
	if !rl.IsTextureValid(srcTex) {
		return rl.Texture2D{}
	}
	srcImg := rl.LoadImageFromTexture(srcTex)
	if srcImg == nil {
		return rl.Texture2D{}
	}
	defer rl.UnloadImage(srcImg)

	w := srcImg.Width
	h := srcImg.Height
	whiteImg := rl.GenImageColor(int(w), int(h), rl.Blank)
	defer rl.UnloadImage(whiteImg)

	for y := int32(0); y < h; y++ {
		for x := int32(0); x < w; x++ {
			c := rl.GetImageColor(*srcImg, x, y)
			if c.A > 0 {
				rl.ImageDrawPixel(whiteImg, x, y, rl.NewColor(255, 255, 255, c.A))
			}
		}
	}

	tex := rl.LoadTextureFromImage(whiteImg)
	if rl.IsTextureValid(tex) {
		rl.SetTextureFilter(tex, rl.FilterPoint)
		r.silhouetteCache[atlasIndex] = tex
	}
	return tex
}

func (r *Renderer) renderSelectionBufferOffscreen(world donburi.World, currentRotation rotation.Rotation, camScreenX, camScreenY, zoomLevel float64, screenW, screenH int) {
	if !rl.IsRenderTextureValid(r.selectionRT) {
		return
	}

	rl.BeginTextureMode(r.selectionRT)
	rl.ClearBackground(rl.Blank)

	forEachTile(world, currentRotation, func(tile renderTile) {
		isSelectable := tile.layer == buildings.KindBuildingsID || tile.layer == buildings.KindRoadsID
		if !isSelectable || tile.buildingID == 0 {
			return
		}
		if tile.srcW <= 0 || tile.srcH <= 0 {
			return
		}
		silhouette := r.getSilhouetteTexture(tile.atlasIndex)
		if !rl.IsTextureValid(silhouette) {
			return
		}

		src := rl.NewRectangle(float32(tile.srcX), float32(tile.srcY), float32(tile.srcW), float32(tile.srcH))
		src = insetRect(src, 0.5)
		if src.Width <= 0 || src.Height <= 0 {
			return
		}

		relX := tile.isoX - camScreenX
		relY := tile.isoY - camScreenY
		screenX := float32(relX*zoomLevel + float64(screenW)/2*(1-zoomLevel))
		screenY := float32(relY*zoomLevel + float64(screenH)/2*(1-zoomLevel))
		zoomScale := float32(zoomLevel)
		size := rl.NewVector2(src.Width*zoomScale, src.Height*zoomScale)

		tintColor := idToEncodedColor(tile.buildingID)
		rl.DrawTexturePro(
			silhouette,
			src,
			rl.NewRectangle(screenX, screenY, size.X, size.Y),
			rl.Vector2Zero(),
			0,
			tintColor,
		)
	})

	rl.EndTextureMode()
}

func (r *Renderer) renderDisplayBuffer(world donburi.World, currentRotation rotation.Rotation, camScreenX, camScreenY, zoomLevel float64, screenW, screenH int) {
	forEachTile(world, currentRotation, func(tile renderTile) {
		isSelectable := tile.layer == buildings.KindBuildingsID || tile.layer == buildings.KindRoadsID
		if !isSelectable || tile.buildingID == 0 {
			return
		}
		if tile.srcW <= 0 || tile.srcH <= 0 {
			return
		}
		silhouette := r.getSilhouetteTexture(tile.atlasIndex)
		if !rl.IsTextureValid(silhouette) {
			return
		}

		src := rl.NewRectangle(float32(tile.srcX), float32(tile.srcY), float32(tile.srcW), float32(tile.srcH))
		src = insetRect(src, 0.5)
		if src.Width <= 0 || src.Height <= 0 {
			return
		}

		relX := tile.isoX - camScreenX
		relY := tile.isoY - camScreenY
		screenX := float32(relX*zoomLevel + float64(screenW)/2*(1-zoomLevel))
		screenY := float32(relY*zoomLevel + float64(screenH)/2*(1-zoomLevel))
		zoomScale := float32(zoomLevel)
		size := rl.NewVector2(src.Width*zoomScale, src.Height*zoomScale)

		tintColor := idToRainbowColor(tile.buildingID)
		rl.DrawTexturePro(
			silhouette,
			src,
			rl.NewRectangle(screenX, screenY, size.X, size.Y),
			rl.Vector2Zero(),
			0,
			tintColor,
		)
	})
}

func (r *Renderer) SampleSelectionBuffer(w *world.World) {
	if !rl.IsRenderTextureValid(r.selectionRT) {
		return
	}

	mouseX := int32(rl.GetMouseX())
	mouseY := int32(rl.GetMouseY())

	if mouseX < 0 || mouseY < 0 || mouseX >= int32(r.selectionRTW) || mouseY >= int32(r.selectionRTH) {
		return
	}

	img := rl.LoadImageFromTexture(r.selectionRT.Texture)
	if img == nil {
		return
	}
	defer rl.UnloadImage(img)

	// Render texture is vertically flipped
	flippedY := int32(r.selectionRTH) - 1 - mouseY
	pixelColor := rl.GetImageColor(*img, mouseX, flippedY)
	buildingID := colorToID(pixelColor)

	controlQuery := donburi.NewQuery(filter.Contains(components.ControlType))
	controlQuery.Each(w.World, func(entry *donburi.Entry) {
		ctrl := components.ControlType.Get(entry)
		ctrl.HoveredBuildingID = buildingID
	})
}

func (r *Renderer) Close() error {
	if r.initialized {
		for _, texture := range r.textures {
			rl.UnloadTexture(texture)
		}
		for _, texture := range r.atlasTextures {
			if rl.IsTextureValid(texture) {
				rl.UnloadTexture(texture)
			}
		}
		for _, tex := range r.silhouetteCache {
			if rl.IsTextureValid(tex) {
				rl.UnloadTexture(tex)
			}
		}
		if rl.IsRenderTextureValid(r.selectionRT) {
			rl.UnloadRenderTexture(r.selectionRT)
		}
		if rl.IsRenderTextureValid(r.displayRT) {
			rl.UnloadRenderTexture(r.displayRT)
		}
		rl.CloseWindow()
		r.initialized = false
	}
	return nil
}

// ShouldClose returns true if the window should close
func (r *Renderer) ShouldClose() bool {
	if !r.initialized {
		return false
	}
	return rl.WindowShouldClose()
}

func (r *Renderer) TakeScreenshot(path string) error {
	if path == "" {
		return errors.New("empty screenshot path")
	}
	if err := os.MkdirAll(filepath.Dir(path), 0o755); err != nil {
		return err
	}
	// Fallback to regular screenshot for now - render texture screenshot has issues
	rl.TakeScreenshot(path)
	return nil
}

func (r *Renderer) SampleSelection(w *world.World) {
	r.SampleSelectionBuffer(w)
}

var errInvisibleImage = errors.New("invisible image")

func (r *Renderer) textureFor(img *ebiten.Image) (rl.Texture2D, error) {
	if img == nil {
		return rl.Texture2D{}, errors.New("nil image")
	}
	if _, failed := r.failedImages[img]; failed {
		return rl.Texture2D{}, errors.New("previously failed")
	}
	if img.Bounds().Dx() == 0 || img.Bounds().Dy() == 0 {
		r.failedImages[img] = struct{}{}
		return rl.Texture2D{}, errors.New("empty image")
	}
	if texture, ok := r.textures[img]; ok {
		return texture, nil
	}
	rgba, visible := r.imageFromEbiten(img)
	if !visible {
		r.invisibleImages[img] = struct{}{}
		return rl.Texture2D{}, errInvisibleImage
	}
	if rgba == nil {
		r.failedImages[img] = struct{}{}
		return rl.Texture2D{}, errors.New("invalid image data")
	}
	if rgba.Bounds().Dx() == 0 || rgba.Bounds().Dy() == 0 {
		r.failedImages[img] = struct{}{}
		return rl.Texture2D{}, errors.New("empty image data")
	}
	imgData := rl.NewImageFromImage(rgba)
	texture := rl.LoadTextureFromImage(imgData)
	rl.UnloadImage(imgData)
	if !rl.IsTextureValid(texture) {
		r.failedImages[img] = struct{}{}
		return rl.Texture2D{}, errors.New("texture load failed")
	}
	r.textures[img] = texture
	return texture, nil
}

func (r *Renderer) imageFromEbiten(img *ebiten.Image) (*image.RGBA, bool) {
	if cached, ok := r.imageCache[img]; ok {
		return cached, true
	}
	if _, ok := r.invisibleImages[img]; ok {
		return nil, false
	}
	width := img.Bounds().Dx()
	height := img.Bounds().Dy()
	if width == 0 || height == 0 {
		return nil, false
	}
	pixels := make([]byte, width*height*4)
	img.ReadPixels(pixels)
	visible := false
	for i := 3; i < len(pixels); i += 4 {
		if pixels[i] != 0 {
			visible = true
			break
		}
	}
	if !visible {
		return nil, false
	}
	rgba := image.NewRGBA(image.Rect(0, 0, width, height))
	copy(rgba.Pix, pixels)
	r.imageCache[img] = rgba
	return rgba, true
}

type renderTile struct {
	isoX             float64
	isoY             float64
	depth            float64
	image            *ebiten.Image
	pivotX           float64
	pivotY           float64
	buildingID       int
	buildingRotation rotation.Rotation
	worldRotation    rotation.Rotation
	animFrame        int
	layer            string
	posX             float64
	posY             float64
	posOffset        float64
	atlasIndex       int
	srcX             int
	srcY             int
	srcW             int
	srcH             int
	rotX             float64
	rotY             float64
	sizeW            int
	sizeH            int
}

func forEachTile(world donburi.World, currentRotation rotation.Rotation, handle func(tile renderTile)) {
	var worldWidth, worldHeight int = 500, 500
	worldQueryCached := donburi.NewQuery(filter.Contains(components.WorldType))
	worldQueryCached.Each(world, func(entry *donburi.Entry) {
		worldComp := components.WorldType.Get(entry)
		if worldComp != nil {
			worldWidth = worldComp.Width
			worldHeight = worldComp.Height
		}
	})

	query := donburi.NewQuery(filter.Contains(components.IslandType))
	query.Each(world, func(entry *donburi.Entry) {
		island := components.IslandType.Get(entry)

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

		islandIsoX := (rotatedIslandX - rotatedIslandY) * (float64(zoom.TileSize()) / 2)
		islandIsoY := (rotatedIslandX + rotatedIslandY) * (float64(zoom.TileHeight()) / 2)

		layerOrder := []string{
			buildings.KindSeaID,
			buildings.KindGroundID + "_OVERLAY",
			buildings.KindGroundID,
			buildings.KindRoadsID,
			buildings.KindForrestID,
			buildings.KindBuildingsID,
		}

		for _, layerID := range layerOrder {
			for _, tileEntry := range island.Tiles[layerID] {
				pos := components.PositionType.Get(tileEntry)
				tile := components.TileType.Get(tileEntry)
				if tile == nil || tile.Image == nil {
					continue
				}

				bld := components.BuildingType.Get(tileEntry)
				if layerID == buildings.KindSeaID {
					if bld != nil && bld.BuildingID == 1201 {
						continue
					}
				}
				anim := components.AnimationType.Get(tileEntry)
				buildingID := 0
				buildingRotation := rotation.DEG0
				if bld != nil {
					buildingID = bld.BuildingID
					buildingRotation = bld.Rotation
				}
				animFrame := 0
				if anim != nil {
					animFrame = anim.CurrentFrame
				}

				lx := int(pos.X - island.X)
				ly := int(pos.Y - island.Y)
				rxl, ryl := rotation.RotatePosition(lx, ly, island.Width, island.Height, currentRotation)
				worldRotX, worldRotY := rotation.RotateWorldPosition(pos.X, pos.Y, worldWidth, worldHeight, currentRotation)

				localX := float64(rxl)
				localY := float64(ryl)
				originX := (localX-localY)*(float64(zoom.TileSize())/2) + islandIsoX
				originY := (localX+localY)*(float64(zoom.TileHeight())/2) + islandIsoY
				originY -= pos.Offset

				drawX := originX - float64(tile.PivotX)
				drawY := originY - float64(tile.PivotY)

				handle(renderTile{
					isoX:             drawX,
					isoY:             drawY,
					depth:            drawY + float64(tile.Size.Z),
					image:            tile.Image,
					pivotX:           float64(tile.PivotX),
					pivotY:           float64(tile.PivotY),
					buildingID:       buildingID,
					buildingRotation: buildingRotation,
					worldRotation:    currentRotation,
					animFrame:        animFrame,
					layer:            layerID,
					posX:             pos.X,
					posY:             pos.Y,
					posOffset:        pos.Offset,
					atlasIndex:       tile.AtlasIndex,
					srcX:             tile.SrcX,
					srcY:             tile.SrcY,
					srcW:             tile.SrcW,
					srcH:             tile.SrcH,
					rotX:             worldRotX,
					rotY:             worldRotY,
					sizeW:            tile.Size.Width,
					sizeH:            tile.Size.Height,
				})
			}
		}
	})
}

func (r *Renderer) SetAtlas(atlasObj *atlas.TextureAtlas) {
	if atlasObj == nil {
		r.atlasImages = nil
		r.atlasVersion = 0
		return
	}
	r.atlasImages = atlasObj.Images
	r.atlasVersion = atlasObj.AtlasMeta.Version
}

func (r *Renderer) SetAtlasPath(path string) {
	r.atlasPath = path
}

func (r *Renderer) ensureAtlasTextures() {
	if r.atlasPath != "" {
		r.ensureAtlasTexturesFromDisk()
		return
	}
	if len(r.atlasImages) == 0 {
		return
	}
	if r.currentAtlas == r.atlasVersion && len(r.atlasTextures) > 0 {
		return
	}
	for _, tex := range r.atlasTextures {
		if rl.IsTextureValid(tex) {
			rl.UnloadTexture(tex)
		}
	}
	r.atlasTextures = make([]rl.Texture2D, len(r.atlasImages))
	for i, img := range r.atlasImages {
		if img == nil {
			continue
		}
		bounds := img.Bounds()
		if bounds.Dx() <= 0 || bounds.Dy() <= 0 {
			log.Printf("raylib atlas texture skipped: index=%d size=%dx%d", i, bounds.Dx(), bounds.Dy())
			continue
		}
		pixels := make([]color.RGBA, bounds.Dx()*bounds.Dy())
		idx := 0
		for y := bounds.Min.Y; y < bounds.Max.Y; y++ {
			for x := bounds.Min.X; x < bounds.Max.X; x++ {
				r, g, b, a := img.At(x, y).RGBA()
				pixels[idx] = color.RGBA{R: uint8(r >> 8), G: uint8(g >> 8), B: uint8(b >> 8), A: uint8(a >> 8)}
				idx++
			}
		}
		imgData := rl.GenImageColor(bounds.Dx(), bounds.Dy(), rl.Black)
		loaded := rl.LoadTextureFromImage(imgData)
		rl.UnloadImage(imgData)
		if rl.IsTextureValid(loaded) {
			rl.UpdateTexture(loaded, pixels)
		}
		r.atlasTextures[i] = loaded
		if rl.IsTextureValid(r.atlasTextures[i]) {
			rl.SetTextureFilter(r.atlasTextures[i], rl.FilterPoint)
			rl.SetTextureWrap(r.atlasTextures[i], rl.WrapClamp)
		} else {
			log.Printf("raylib atlas texture failed: index=%d size=%dx%d", i, bounds.Dx(), bounds.Dy())
		}
	}
	r.currentAtlas = r.atlasVersion
}

func (r *Renderer) ensureAtlasTexturesFromDisk() {
	base := strings.TrimSuffix(r.atlasPath, ".json")
	if base == r.atlasPath {
		base = strings.TrimSuffix(r.atlasPath, ".png")
	}
	if base == r.currentAtlasKey && len(r.atlasTextures) > 0 {
		return
	}
	for _, tex := range r.atlasTextures {
		if rl.IsTextureValid(tex) {
			rl.UnloadTexture(tex)
		}
	}
	r.atlasTextures = nil
	for i := 0; ; i++ {
		path := fmt.Sprintf("%s-%04d.png", base, i)
		if _, err := os.Stat(path); err != nil {
			if i == 0 {
				log.Printf("raylib atlas texture missing: %s", path)
			}
			break
		}
		tex := rl.LoadTexture(path)
		if rl.IsTextureValid(tex) {
			rl.SetTextureFilter(tex, rl.FilterPoint)
			rl.SetTextureWrap(tex, rl.WrapClamp)
			r.atlasTextures = append(r.atlasTextures, tex)
			log.Printf("raylib atlas texture loaded: %s", path)
		} else {
			log.Printf("raylib atlas texture failed: %s", path)
		}
	}
	r.currentAtlasKey = base
	log.Printf("raylib atlas textures ready: %d from %s", len(r.atlasTextures), base)
}

func (r *Renderer) ensureIsoCamera(_ Screen, zoomLevel float64, _ *components.Camera) {
	ppu := float32(zoom.TileSize()) * float32(zoomLevel)
	if ppu <= 0 {
		ppu = float32(zoom.TileSize())
	}
	r.ppu = ppu
}

func (r *Renderer) renderHUDOverlay(world donburi.World, camera *components.Camera, currentRotation rotation.Rotation, renderedTiles int) {
	if camera == nil {
		return
	}

	var ctrl *components.Control
	controlQuery := donburi.NewQuery(filter.Contains(components.ControlType))
	controlQuery.Each(world, func(entry *donburi.Entry) {
		ctrl = components.ControlType.Get(entry)
	})

	fontSize := int32(13)
	x := int32(10)
	y := int32(10)
	lineHeight := int32(16)

	fps := rl.GetFPS()
	fpsColor := rl.Green
	if fps < 30 {
		fpsColor = rl.Red
	} else if fps < 55 {
		fpsColor = rl.Yellow
	}
	rl.DrawText(fmt.Sprintf("FPS: %d", fps), x, y, fontSize, fpsColor)
	y += lineHeight

	rl.DrawText(fmt.Sprintf("Tiles: %d drawn", renderedTiles), x, y, fontSize, rl.White)
	y += lineHeight

	rl.DrawText(fmt.Sprintf("Camera: (%.1f, %.1f)", camera.X, camera.Y), x, y, fontSize, rl.White)
	y += lineHeight

	rl.DrawText(fmt.Sprintf("Zoom: %.0f%%", camera.Zoom*100), x, y, fontSize, rl.White)
	y += lineHeight

	rl.DrawText(fmt.Sprintf("Rotation: %s", currentRotation.String()), x, y, fontSize, rl.White)
	y += lineHeight

	if ctrl != nil {
		rl.DrawText(fmt.Sprintf("Mouse Tile: (%.1f, %.1f)", ctrl.MouseTileX, ctrl.MouseTileY), x, y, fontSize, rl.White)
		y += lineHeight

		if ctrl.HoveredIsland >= 0 {
			rl.DrawText(fmt.Sprintf("Island: %d", ctrl.HoveredIsland), x, y, fontSize, rl.Yellow)
		} else {
			rl.DrawText("Island: -", x, y, fontSize, rl.White)
		}
		y += lineHeight

		if ctrl.HoveredBuildingID > 0 {
			rl.DrawText(fmt.Sprintf("Building: %d", ctrl.HoveredBuildingID), x, y, fontSize, rl.Yellow)
		} else {
			rl.DrawText("Building: -", x, y, fontSize, rl.White)
		}
		y += lineHeight

		if ctrl.HoveredTileID >= 0 {
			rl.DrawText(fmt.Sprintf("Tile ID: %d", ctrl.HoveredTileID), x, y, fontSize, rl.Yellow)
		} else {
			rl.DrawText("Tile ID: -", x, y, fontSize, rl.White)
		}
	}
}

func insetRect(src rl.Rectangle, px float32) rl.Rectangle {
	return rl.NewRectangle(src.X+px, src.Y+px, src.Width-2*px, src.Height-2*px)
}

func TileToScreen(tileX, tileY float64, tileWidth, tileHeight int) (screenX, screenY float64) {
	screenX = (tileX - tileY) * (float64(tileWidth) / 2)
	screenY = (tileX + tileY) * (float64(tileHeight) / 2)
	return
}

func ScreenToTile(screenX, screenY float64, tileWidth, tileHeight int) (tileX, tileY float64) {
	tw := float64(tileWidth) / 2
	th := float64(tileHeight) / 2
	tileX = (screenX/tw + screenY/th) / 2
	tileY = (screenY/th - screenX/tw) / 2
	return
}
