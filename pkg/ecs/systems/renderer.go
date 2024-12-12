package systems

import (
	"github.com/hajimehoshi/ebiten/v2"
	"github.com/siredmar/mdcii-engine/pkg/ecs/components"
	"github.com/siredmar/mdcii-engine/pkg/world/zoom"
	"github.com/yohamta/donburi"
	"github.com/yohamta/donburi/filter"
)

// Define a query using filter.LayoutFilter
var rendererQuery = donburi.NewQuery(
	filter.Contains(components.IslandType),
)

func RenderSystem(world donburi.World, screen *ebiten.Image) {
	tileWidth := zoom.TileSize()
	tileHeight := zoom.TileHeight()
	rendererQuery.Each(world, func(entry *donburi.Entry) {
		island := components.IslandType.Get(entry)
		for y := 0; y < island.Height; y++ {
			for x := 0; x < island.Width; x++ {
				tileEntry := island.Tiles[y][x]
				pos := components.PositionType.Get(tileEntry)
				tile := components.TileType.Get(tileEntry)
				if tile.Image != nil {
					isoX := ((pos.X-island.X)-(pos.Y-island.Y))*(float64(tileWidth)/2) + (island.X * (float64(tileWidth) / 2))
					isoY := ((pos.X-island.X)+(pos.Y-island.Y))*(float64(tileHeight)/2) + (island.Y * (float64(tileHeight) / 2))
					isoY -= pos.Offset

					// Adjust Y position so the bottom of the tile aligns
					// Assuming `tile.Image` is an *ebiten.Image, we can get its height
					tileImageHeight := float64(tile.Image.Bounds().Dy())
					isoY -= tileImageHeight - float64(tileHeight)

					op := &ebiten.DrawImageOptions{}
					op.GeoM.Translate(isoX, isoY)
					screen.DrawImage(tile.Image, op)
				}
			}
		}
	})
}
