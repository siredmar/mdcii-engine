package systems

import (
	"github.com/hajimehoshi/ebiten/v2"
	"github.com/siredmar/mdcii-engine/pkg/ecs/components"
	"github.com/yohamta/donburi"
	"github.com/yohamta/donburi/filter"
)

// Define a query using filter.LayoutFilter
var rendererQuery = donburi.NewQuery(
	filter.Contains(components.PositionType, components.TileType),
)

func RenderSystem(world donburi.World, screen *ebiten.Image) {
	rendererQuery.Each(world, func(entry *donburi.Entry) {
		tile := components.TileType.Get(entry)
		if tile.Image != nil {
			pos := components.PositionType.Get(entry)
			op := &ebiten.DrawImageOptions{}
			op.GeoM.Translate(pos.X, pos.Y)
			screen.DrawImage(tile.Image, op)
		}
	})
}
