package tiles

import (
	"github.com/hajimehoshi/ebiten/v2"
	"github.com/siredmar/mdcii-engine/pkg/cod/buildings"
	errors "github.com/siredmar/mdcii-engine/pkg/errors"
	math "github.com/siredmar/mdcii-engine/pkg/math"
	"github.com/siredmar/mdcii-engine/pkg/texture/sprites"
	"github.com/siredmar/mdcii-engine/pkg/world/camera"
	"github.com/siredmar/mdcii-engine/pkg/world/rotation"
	"github.com/siredmar/mdcii-engine/pkg/world/zoom"
)

type TerrainTile struct {
	Rotation      rotation.Rotation   `json:"rotation"`
	X             int                 `json:"x"`
	Y             int                 `json:"y"`
	Building      *buildings.Building `json:"building"`
	TileType      TileType            `json:"tileType"`
	Gfx           []int               `json:"gfx"`
	Frame         int                 `json:"frame"`
	RenderIndices []int               `json:"renderIndices"`
	Sprites       map[int]sprites.Sprite
	op            *ebiten.DrawImageOptions
}

type TerrainTileOption func(*TerrainTile)

func WithTileType(tileType TileType) func(*TerrainTile) {
	return func(t *TerrainTile) {
		t.TileType = tileType
	}
}

func NewTerrainTile(r rotation.Rotation, x int, y int, building *buildings.Building, s *sprites.Sprites, opts ...TerrainTileOption) *TerrainTile {
	t := &TerrainTile{
		Rotation:      r,
		X:             x,
		Y:             y,
		Building:      building,
		TileType:      TerrainTypeNone,
		Gfx:           []int{},
		Frame:         0,
		RenderIndices: []int{},
		op:            &ebiten.DrawImageOptions{},
		Sprites:       map[int]sprites.Sprite{},
	}
	for _, opt := range opts {
		opt(t)
	}

	// building := g.buildings.Buildings[t.Id]
	// 		tile := tiles.NewTerrainTile(rotation.Rotation(t.Orientation), t.Posx, t.Posy, g.buildings.Buildings[t.Id], g.gfxSprites)

	t.CalculateGfxValues()
	t.CalcRenderPositions(building.Size.W, building.Size.H)
	t.Sprites = s.Sprites
	// add gfx indices for each rotation for this tile
	// for _, i := range t.Gfx {
	// 	spriteIndex := i
	// 	t.Sprites = append(t.Sprites, s.Sprites[spriteIndex])
	// }
	return t
}

func (t *TerrainTile) CalculateGfxValues() {
	if t.Building != nil {
		gfx0 := t.Building.Gfx
		t.Gfx = append(t.Gfx, gfx0)
		if t.Building.IsRotatable() {
			t.Gfx = append(t.Gfx, gfx0+(1*t.Building.Rotate))
			t.Gfx = append(t.Gfx, gfx0+(2*t.Building.Rotate))
			t.Gfx = append(t.Gfx, gfx0+(3*t.Building.Rotate))
		} else {
			t.Gfx = append(t.Gfx, gfx0)
			t.Gfx = append(t.Gfx, gfx0)
			t.Gfx = append(t.Gfx, gfx0)
		}
		if t.Building.IsBig() {
			for i, gfx := range t.Gfx {
				t.Gfx[i] = t.AdjustGfxForBigBuildings(gfx)
			}
		}

	}
}

// Assuming HasBuilding method for TerrainTile
func (tile *TerrainTile) HasBuilding() bool {
	return tile.Building != nil
}

func (tile *TerrainTile) AdjustGfxForBigBuildings(gfx int) int {
	errors.MDCII_ASSERT(tile.HasBuilding(), "[TerrainTile::AdjustGfxForBigBuildings()] nil")

	// default: orientation 0
	rp := math.Vector2D{X: tile.X, Y: tile.Y}

	switch tile.Rotation {
	case rotation.DEG270:
		rp.X, rp.Y = rotation.RotatePosition(tile.X, tile.Y, tile.Building.Size.W, tile.Building.Size.H, rotation.DEG90)
	case rotation.DEG180:
		rp.X, rp.Y = rotation.RotatePosition(tile.X, tile.Y, tile.Building.Size.W, tile.Building.Size.H, rotation.DEG180)
	case rotation.DEG90:
		rp.X, rp.Y = rotation.RotatePosition(tile.X, tile.Y, tile.Building.Size.W, tile.Building.Size.H, rotation.DEG270)
	}

	offset := rp.Y*tile.Building.Size.W + rp.X
	gfx += offset
	return gfx
}

func (t *TerrainTile) HasBuildingAboveWaterAndCoast() bool {
	return t.Building != nil && t.Building.PositionOffset > 0
}

func (t *TerrainTile) GetRenderIndex(width, height int, r rotation.Rotation) int {
	// errors.MDCII_ASSERT(t.X >= 0 && t.X < width, "[Tile::GetRenderIndex()] Invalid x position given.")
	// errors.MDCII_ASSERT(t.Y >= 0 && t.Y < height, "[Tile::GetRenderIndex()] Invalid y position given.")

	posX, posY := rotation.RotatePosition(t.X, t.Y, width, height, r)

	if r == rotation.DEG0 || r == rotation.DEG180 {
		return posY*width + posX
	}

	return posY*height + posX
}

func (t *TerrainTile) CalcRenderPositions(width, height int) {
	t.RenderIndices = make([]int, 4)
	t.RenderIndices[0] = t.GetRenderIndex(width, height, rotation.DEG0)
	t.RenderIndices[1] = t.GetRenderIndex(width, height, rotation.DEG90)
	t.RenderIndices[2] = t.GetRenderIndex(width, height, rotation.DEG180)
	t.RenderIndices[3] = t.GetRenderIndex(width, height, rotation.DEG270)
}

func (t *TerrainTile) CalcOffset(r rotation.Rotation) float32 {
	var offset float32 = 0.0

	// zoomInt := int(magic_enum.EnumInteger(atlas.world.Camera.Zoom))
	// tileHeight := 0
	// tileHeight := atlas.getTileHeight(atlas.world.Camera.Zoom)

	// gfxHeight := t.heights[zoomInt][tGfx]

	// if atlas.world.Camera.Zoom == world.ZoomGFX {

	tileHeight := zoom.TileHeight()
	// tileHeight = 31
	// }

	h := t.Sprites[t.Gfx[t.Rotation]].Height
	if h > tileHeight {
		offset = float32(h) - float32(tileHeight)
	}

	if t.HasBuildingAboveWaterAndCoast() {
		offset += zoom.Elevation()
	}

	return offset
}

func (t *TerrainTile) Render(r rotation.Rotation, screen *ebiten.Image) error {
	// if g.tileInfoX == x && g.tileInfoY == y {
	// 	ebitenutil.DebugPrintAt(screen, fmt.Sprintf("Tile: %d, X: %d, Y: %d, Orientation: %d, GFX: %d, PosOffset: %d", t.Id, x, y, t.Orientation, tile.Gfx[tile.Rotation], building.PositionOffset), 0, 40)
	// 	ebitenutil.DebugPrintAt(screen, fmt.Sprintf("Type: %s", building.Kind.String()), 0, 60)
	// }
	camera := camera.GetCamera()
	xi, yi := rotation.CartesianToIso(float64(t.X), float64(t.Y), zoom.TileSize())
	t.op.GeoM.Reset()
	//Translate for isometric
	t.op.GeoM.Translate(float64(xi), float64(yi))
	// Translate for tile offset
	t.op.GeoM.Translate(0, -float64(t.CalcOffset(r)))
	//Scale for camera zoom
	t.op.GeoM.Scale(camera.Zoom, camera.Zoom)
	//Translate for center of screen offset
	// op.GeoM.Translate(float64(g.windowWidth/2.0), float64(g.windowHeight/2.0))
	//Translate for camera position
	t.op.GeoM.Translate(-camera.X, camera.Y)
	// gridOp := &ebiten.DrawImageOptions{}
	// gridOp.GeoM.Translate(float64(xi), float64(yi))
	// gridOp.GeoM.Scale(g.Camera.Zoom, g.Camera.Zoom)
	// gridOp.GeoM.Translate(float64(g.windowWidth/2.0), float64(g.windowHeight/2.0))
	// gridOp.GeoM.Translate(-g.Camera.X, g.Camera.Y)

	img := t.Sprites[t.Gfx[t.Rotation]].Image
	// img := t.Sprites.Sprites[t.Gfx[t.Rotation]].Image
	// img := t.Sprites[t.Rotation].Image
	screen.DrawImage(img, t.op)
	return nil
}
