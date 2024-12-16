package components

import (
	"fmt"

	"github.com/siredmar/mdcii-engine/pkg/building"
	island5 "github.com/siredmar/mdcii-engine/pkg/chunks"
	"github.com/siredmar/mdcii-engine/pkg/world/rotation"
	"github.com/yohamta/donburi"

	animations "github.com/siredmar/mdcii-engine/pkg/texture/animations"
)

type Island struct {
	Width, Height int              // Dimensions of the island
	X, Y          float64          // Position of the island in the world
	Tiles         []*donburi.Entry // 2D grid of tile entries
}

var IslandType = donburi.NewComponentType[Island]()

// func CreateIsland(world donburi.World, ani *animations.Animations, width, height int, worldX, worldY float64) *donburi.Entry {
// 	// Create the Island entity
// 	islandEntity := world.Create(IslandType)
// 	islandEntry := world.Entry(islandEntity)

// 	// Initialize the Island component
// 	island := &Island{
// 		Width:  width,
// 		Height: height,
// 		X:      worldX,
// 		Y:      worldY,
// 		Tiles:  make([][]*donburi.Entry, height),
// 	}

// 	// Populate the island with tiles
// 	for y := 0; y < height; y++ {
// 		island.Tiles[y] = make([]*donburi.Entry, width)
// 		for x := 0; x < width; x++ {
// 			tileEntity := world.Create(BuildingType, PositionType, TileType, AnimationType)
// 			tileEntry := world.Entry(tileEntity)

// 			// Set components for the tile
// 			PositionType.Set(tileEntry, &Position{X: worldX + float64(x), Y: worldY + float64(y)})
// 			TileType.Set(tileEntry, &Tile{})
// 			anim := ani.GetAnimation(1804, rotation.DEG0)
// 			AnimationType.Set(tileEntry, &Animation{
// 				Count:        anim.Steps,
// 				Duration:     float64(anim.FrameDuration),
// 				Running:      true,
// 				Loop:         true,
// 				CurrentFrame: 0,
// 				CurrentTime:  0,
// 			})
// 			BuildingType.Set(tileEntry, &Building{BuildingID: 1804, Rotation: rotation.DEG0})

// 			// Add the tile entry to the island grid
// 			island.Tiles[y][x] = tileEntry
// 		}
// 	}

// 	// Set the Island component
// 	IslandType.Set(islandEntry, island)

// 	return islandEntry
// }

func CreateIslandFromChunk(world donburi.World, ani *animations.Animations, i *island5.Island5, worldX, worldY float64) *donburi.Entry {
	// Create the Island entity
	islandEntity := world.Create(IslandType)
	islandEntry := world.Entry(islandEntity)

	// Initialize the Island component
	island := &Island{
		Width:  i.Width,
		Height: i.Height,
		X:      worldX,
		Y:      worldY,
		Tiles:  []*donburi.Entry{},
	}
	// Initialize the set grid to track placed tiles
	set := make([][]bool, i.Layers.Top.Size.Height)
	for y := 0; y < i.Layers.Top.Size.Height; y++ {
		set[y] = make([]bool, i.Width) // Initialize each row
	}

	for _, field := range i.Layers.Top.Fields {
		x := field.Posx
		y := field.Posy

		// if set[y][x] {
		// 	continue
		// }

		currentTile := field
		tileEntity := world.Create(BuildingType, PositionType, TileType, AnimationType)
		tileEntry := world.Entry(tileEntity)
		posOffset := i.Buildings.Buildings[currentTile.Id].PositionOffset
		size := i.Buildings.Buildings[currentTile.Id].Size
		// Set components for the tile
		PositionType.Set(tileEntry, &Position{X: worldX + float64(x), Y: worldY + float64(y), Offset: float64(posOffset)})
		TileType.Set(tileEntry, &Tile{
			Size: Size{Width: size.W, Height: size.H},
		})
		anim := ani.GetAnimation(currentTile.Id, rotation.DEG0)
		if currentTile.Id != 0xFFFF {
			set[y][x] = true
		}
		AnimationType.Set(tileEntry, &Animation{
			Count:        anim.Steps,
			Duration:     float64(anim.FrameDuration),
			Running:      true,
			Loop:         true,
			CurrentFrame: 0,
			CurrentTime:  0,
		})
		BuildingType.Set(tileEntry, &Building{
			BuildingID: currentTile.Id,
			Rotation:   rotation.Rotation(currentTile.Orientation),
			Size:       building.BuildingSize(size.W, size.H),
		})

		// Add the tile entry to the island grid
		island.Tiles = append(island.Tiles, tileEntry)
	}

	for y := 0; y < island.Height; y++ {
		for x := 0; x < island.Width; x++ {
			if set[y][x] {
				fmt.Printf("+")
			} else {
				fmt.Printf(" ")
			}
		}
		fmt.Println()
	}
	// Set the Island component
	IslandType.Set(islandEntry, island)

	return islandEntry
}
