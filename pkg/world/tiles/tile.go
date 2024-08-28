package tiles

type Tile interface {
	Render()
	CalculateGfxValues()
}

type TileType int

const (
	TerrainTypeNone TileType = iota
	TerrainTypeResidential
	TerrainTypeTraffic
	TerrainTypePlants
)
