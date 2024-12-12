package components

import (
	island5 "github.com/siredmar/mdcii-engine/pkg/chunks"
	"github.com/siredmar/mdcii-engine/pkg/world/rotation"
	"github.com/yohamta/donburi"

	animations "github.com/siredmar/mdcii-engine/pkg/texture/animations"
)

type Island struct {
	Width, Height int                // Dimensions of the island
	X, Y          float64            // Position of the island in the world
	Tiles         [][]*donburi.Entry // 2D grid of tile entries
}

var IslandType = donburi.NewComponentType[Island]()

func CreateIsland(world donburi.World, ani *animations.Animations, width, height int, worldX, worldY float64) *donburi.Entry {
	// Create the Island entity
	islandEntity := world.Create(IslandType)
	islandEntry := world.Entry(islandEntity)

	// Initialize the Island component
	island := &Island{
		Width:  width,
		Height: height,
		X:      worldX,
		Y:      worldY,
		Tiles:  make([][]*donburi.Entry, height),
	}

	// Populate the island with tiles
	for y := 0; y < height; y++ {
		island.Tiles[y] = make([]*donburi.Entry, width)
		for x := 0; x < width; x++ {
			tileEntity := world.Create(BuildingType, PositionType, TileType, AnimationType)
			tileEntry := world.Entry(tileEntity)

			// Set components for the tile
			PositionType.Set(tileEntry, &Position{X: worldX + float64(x), Y: worldY + float64(y)})
			TileType.Set(tileEntry, &Tile{})
			anim := ani.GetAnimation(1804, rotation.DEG0)
			AnimationType.Set(tileEntry, &Animation{
				Count:        anim.Steps,
				Duration:     float64(anim.FrameDuration),
				Running:      true,
				Loop:         true,
				CurrentFrame: 0,
				CurrentTime:  0,
			})
			BuildingType.Set(tileEntry, &Building{BuildingID: 1804, Rotation: rotation.DEG0})

			// Add the tile entry to the island grid
			island.Tiles[y][x] = tileEntry
		}
	}

	// Set the Island component
	IslandType.Set(islandEntry, island)

	return islandEntry
}

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
		Tiles:  make([][]*donburi.Entry, i.Height),
	}
	top := i.Layers.Top
	for y := 0; y < i.Height; y++ {
		island.Tiles[y] = make([]*donburi.Entry, i.Width)
		for x := 0; x < i.Width; x++ {
			currentTile := top.Fields[y*i.Width+x]
			tileEntity := world.Create(BuildingType, PositionType, TileType, AnimationType)
			tileEntry := world.Entry(tileEntity)
			posOffset := i.Buildings.Buildings[currentTile.Id].PositionOffset
			// Set components for the tile
			PositionType.Set(tileEntry, &Position{X: worldX + float64(x), Y: worldY + float64(y), Offset: float64(posOffset)})
			TileType.Set(tileEntry, &Tile{})
			anim := ani.GetAnimation(1804, rotation.DEG0)
			AnimationType.Set(tileEntry, &Animation{
				Count:        anim.Steps,
				Duration:     float64(anim.FrameDuration),
				Running:      true,
				Loop:         true,
				CurrentFrame: 0,
				CurrentTime:  0,
			})
			BuildingType.Set(tileEntry, &Building{BuildingID: currentTile.Id, Rotation: rotation.Rotation(currentTile.Orientation)})

			// Add the tile entry to the island grid
			island.Tiles[y][x] = tileEntry
		}
	}

	// Set the Island component
	IslandType.Set(islandEntry, island)

	return islandEntry
}
