package tiles

import (
	"github.com/siredmar/mdcii-engine/pkg/cod/buildings"
	errors "github.com/siredmar/mdcii-engine/pkg/errors"
	math "github.com/siredmar/mdcii-engine/pkg/math"
	"github.com/siredmar/mdcii-engine/pkg/texture/sprites"
	"github.com/siredmar/mdcii-engine/pkg/world/elevations"
	"github.com/siredmar/mdcii-engine/pkg/world/rotation"
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
	Sprite        sprites.Sprite
}

type TerrainTileOption func(*TerrainTile)

func WithTileType(tileType TileType) func(*TerrainTile) {
	return func(t *TerrainTile) {
		t.TileType = tileType
	}
}

func NewTerrainTile(r rotation.Rotation, x int, y int, building *buildings.Building, sprites *sprites.Sprites, opts ...TerrainTileOption) *TerrainTile {
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
	t.CalculateGfxValues()
	t.CalcRenderPositions(building.Size.W, building.Size.H)
	t.Sprite = sprites.Sprites[t.Gfx[t.Rotation]]
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

func (t *TerrainTile) GetRenderIndex(width, height int, r rotation.Rotation) int {
	errors.MDCII_ASSERT(t.X >= 0 && t.X < width, "[Tile::GetRenderIndex()] Invalid x position given.")
	errors.MDCII_ASSERT(t.Y >= 0 && t.Y < height, "[Tile::GetRenderIndex()] Invalid y position given.")

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

func (t *TerrainTile) CalcOffset() float32 {
	var offset float32 = 0.0

	// zoomInt := int(magic_enum.EnumInteger(atlas.world.Camera.Zoom))
	zoom := 2
	tileHeight := 0
	// tileHeight := atlas.getTileHeight(atlas.world.Camera.Zoom)

	// gfxHeight := t.heights[zoomInt][tGfx]

	// if atlas.world.Camera.Zoom == world.ZoomGFX {
	tileHeight = 31
	// }

	if t.Sprite.Height > tileHeight {
		offset = float32(t.Sprite.Height) - float32(tileHeight)
	}

	if t.HasBuildingAboveWaterAndCoast() {
		offset += elevations.Elevations[zoom]
	}

	return offset
}
