package systems

import (
	"fmt"

	rl "github.com/gen2brain/raylib-go/raylib"
	"github.com/siredmar/mdcii-engine/pkg/ecs/components"
	"github.com/siredmar/mdcii-engine/pkg/world/zoom"
	"github.com/yohamta/donburi"
	"github.com/yohamta/donburi/filter"
)

// Define a query to find all tiles with PositionType and TileType
var tileQuery = donburi.NewQuery(
	filter.Contains(components.IslandType, components.PositionType, components.TileType),
)

// MouseSelectorSystem calculates and interacts with selected tiles
func MouseSelectorSystem(world donburi.World) {
	tileWidth := zoom.TileSize()    // Tile width in pixels
	tileHeight := zoom.TileHeight() // Tile height in pixels

	// Get the mouse position
	mouse := rl.GetMousePosition()
	mouseX, mouseY := int(mouse.X), int(mouse.Y)

	// Reverse isometric projection to get grid coordinates
	gridX, gridY := getMouseTilePosition(mouseX, mouseY, tileWidth, tileHeight)

	// Query tiles in the ECS
	var selectedTile *donburi.Entry
	// var island *components.Island
	tileQuery.Each(world, func(entry *donburi.Entry) {
		pos := components.PositionType.Get(entry)
		if int(pos.X) == gridX && int(pos.Y) == gridY {
			selectedTile = entry
		}
	})
	// Print or interact with the selected tile
	if selectedTile != nil {
		building := components.BuildingType.Get(selectedTile)
		fmt.Printf("Tile Selected: X=%d, Y=%d, TileID=%d\n", gridX-10, gridY, building.BuildingID)
	} else {
		fmt.Printf("No tile at X=%d, Y=%d\n", gridX-10, gridY)
	}
}

// getMouseTilePosition reverses the isometric projection
func getMouseTilePosition(mouseX, mouseY int, tileWidth, tileHeight int) (int, int) {
	worldX := float64(mouseX)
	worldY := float64(mouseY)

	// Reverse isometric projection
	gridX := int((worldX/(float64(tileWidth)/2) + worldY/(float64(tileHeight)/2)) / 2)
	gridY := int((worldY/(float64(tileHeight)/2) - worldX/(float64(tileWidth)/2)) / 2)

	return gridX, gridY
}
