package tiles

import (
	"github.com/siredmar/mdcii-engine/pkg/cod/buildings"
	errors "github.com/siredmar/mdcii-engine/pkg/errors"
	math "github.com/siredmar/mdcii-engine/pkg/math"
	rotation "github.com/siredmar/mdcii-engine/pkg/world/rotation"
)

type TerrainTile struct {
	Rotation      rotation.Rotation   `json:"rotation"`
	X             int                 `json:"x"`
	Y             int                 `json:"y"`
	Building      *buildings.Building `json:"building"`
	TileType      TileType            `json:"tileType"`
	Gfx           []int               `json:"gfx"`
	Frame         int                 `json:"frame"`
	RenderIndices []int
}

type TerrainTileOption func(*TerrainTile)

func WithTileType(tileType TileType) func(*TerrainTile) {
	return func(t *TerrainTile) {
		t.TileType = tileType
	}
}

func NewTerrainTile(r rotation.Rotation, x int, y int, building *buildings.Building, tileType TileType, opts []TerrainTileOption) *TerrainTile {
	t := &TerrainTile{
		Rotation:      r,
		X:             x,
		Y:             y,
		Building:      building,
		TileType:      TerrainTypeNone,
		Gfx:           []int{},
		Frame:         0,
		RenderIndices: []int{},
	}
	for _, opt := range opts {
		opt(t)
	}
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

func (tile *TerrainTile) AdjustGfxForBigBuildings(t_gfx int) int {
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
	t_gfx += offset
	return t_gfx
}

func (t *TerrainTile) HasBuildingAboveWaterAndCoast() bool {
	return t.Building != nil && t.Building.PositionOffset > 0
}

func (t *TerrainTile) GetRenderIndex(x, y, width, height int, r rotation.Rotation) int {
	errors.MDCII_ASSERT(x >= 0 && x < width, "[Tile::GetRenderIndex()] Invalid x position given.")
	errors.MDCII_ASSERT(y >= 0 && y < height, "[Tile::GetRenderIndex()] Invalid y position given.")

	posX, posY := rotation.RotatePosition(x, y, width, height, r)

	if r == rotation.DEG0 || r == rotation.DEG180 {
		return posY*width + posX
	}

	return posY*height + posX
}

func (t *TerrainTile) CalcRenderPositions(width, height int) {
	t.RenderIndices = make([]int, 4)
	t.RenderIndices[0] = t.GetRenderIndex(t.X, t.Y, width, height, rotation.DEG0)
	t.RenderIndices[1] = t.GetRenderIndex(t.X, t.Y, width, height, rotation.DEG90)
	t.RenderIndices[2] = t.GetRenderIndex(t.X, t.Y, width, height, rotation.DEG180)
	t.RenderIndices[3] = t.GetRenderIndex(t.X, t.Y, width, height, rotation.DEG270)
}
