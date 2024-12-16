package systems

import (
	_ "embed"
	"fmt"

	"github.com/siredmar/mdcii-engine/pkg/ecs/components"
	"github.com/yohamta/donburi"
)

// hashToChar maps an integer to a single character
func hashToChar(num int) rune {
	const charset = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#$%^&*()_+-=[]{}|;:',.<>?/`~" // Custom charset
	charsetLength := len(charset)

	// Simple hashing: mod by charset length to get a valid index
	index := num % charsetLength
	return rune(charset[index])
}
func RenderSystemAscii(world donburi.World) {
	rendererQuery.Each(world, func(entry *donburi.Entry) {
		island := components.IslandType.Get(entry)
		chars := make([][]rune, island.Height)

		// Initialize the ASCII grid
		for h := 0; h < island.Height; h++ {
			chars[h] = make([]rune, island.Width)
			for w := 0; w < island.Width; w++ {
				chars[h][w] = ' ' // Default empty character
			}
		}

		// Place buildings or tiles in the grid
		for _, tileEntry := range island.Tiles {
			pos := components.PositionType.Get(tileEntry)
			building := components.BuildingType.Get(tileEntry)

			// Bounds check to avoid out-of-range errors
			if pos.Y >= 0 && int(pos.Y) < island.Height && pos.X >= 0 && int(pos.X) < island.Width {
				chars[int(pos.Y)][int(pos.X)] = hashToChar(building.BuildingID)
			}
		}

		// Print the ASCII grid
		for _, row := range chars {
			for _, char := range row {
				fmt.Printf("%c", char)
			}
			fmt.Println()
		}
		fmt.Println("\n")
	})
}
