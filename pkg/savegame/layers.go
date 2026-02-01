package savegame

import "fmt"

// AddTile adds a tile to the specified layer
func (layers *IslandLayers) AddTile(layerName string, tile Tile) error {
	switch layerName {
	case "sea":
		if layers.Sea == nil {
			layers.Sea = &TileLayer{Tiles: []Tile{}}
		}
		layers.Sea.Tiles = append(layers.Sea.Tiles, tile)
	case "ground":
		if layers.Ground == nil {
			layers.Ground = &TileLayer{Tiles: []Tile{}}
		}
		layers.Ground.Tiles = append(layers.Ground.Tiles, tile)
	case "roads":
		if layers.Roads == nil {
			layers.Roads = &TileLayer{Tiles: []Tile{}}
		}
		layers.Roads.Tiles = append(layers.Roads.Tiles, tile)
	case "forest":
		if layers.Forest == nil {
			layers.Forest = &TileLayer{Tiles: []Tile{}}
		}
		layers.Forest.Tiles = append(layers.Forest.Tiles, tile)
	default:
		return fmt.Errorf("unknown layer: %s", layerName)
	}
	return nil
}

// RemoveTile removes a tile at the specified position from the layer
func (layers *IslandLayers) RemoveTile(layerName string, pos GridPosition) error {
	var tileLayer *TileLayer
	switch layerName {
	case "sea":
		tileLayer = layers.Sea
	case "ground":
		tileLayer = layers.Ground
	case "roads":
		tileLayer = layers.Roads
	case "forest":
		tileLayer = layers.Forest
	default:
		return fmt.Errorf("unknown layer: %s", layerName)
	}

	if tileLayer == nil {
		return nil
	}

	for i, tile := range tileLayer.Tiles {
		if tile.Position.X == pos.X && tile.Position.Y == pos.Y {
			tileLayer.Tiles = append(tileLayer.Tiles[:i], tileLayer.Tiles[i+1:]...)
			return nil
		}
	}
	return nil
}

// GetTileAt returns the tile at the specified position in the layer, or nil
func (layers *IslandLayers) GetTileAt(layerName string, pos GridPosition) *Tile {
	var tileLayer *TileLayer
	switch layerName {
	case "sea":
		tileLayer = layers.Sea
	case "ground":
		tileLayer = layers.Ground
	case "roads":
		tileLayer = layers.Roads
	case "forest":
		tileLayer = layers.Forest
	default:
		return nil
	}

	if tileLayer == nil {
		return nil
	}

	for i := range tileLayer.Tiles {
		if tileLayer.Tiles[i].Position.X == pos.X && tileLayer.Tiles[i].Position.Y == pos.Y {
			return &tileLayer.Tiles[i]
		}
	}
	return nil
}

// AddEntity adds a building entity to the buildings layer
func (layer *BuildingLayer) AddEntity(entity BuildingEntity) {
	if layer.Entities == nil {
		layer.Entities = []BuildingEntity{}
	}
	layer.Entities = append(layer.Entities, entity)
}

// RemoveEntity removes the building entity at the specified origin position
func (layer *BuildingLayer) RemoveEntity(pos GridPosition) bool {
	for i, entity := range layer.Entities {
		if entity.Position.X == pos.X && entity.Position.Y == pos.Y {
			layer.Entities = append(layer.Entities[:i], layer.Entities[i+1:]...)
			return true
		}
	}
	return false
}

// GetEntityAt returns the building entity at the specified origin position, or nil
func (layer *BuildingLayer) GetEntityAt(pos GridPosition) *BuildingEntity {
	for i := range layer.Entities {
		if layer.Entities[i].Position.X == pos.X && layer.Entities[i].Position.Y == pos.Y {
			return &layer.Entities[i]
		}
	}
	return nil
}

// GetEntityCoveringPosition returns the building entity whose footprint covers the position
// Requires footprintLookup function that takes building ID and returns (width, height)
func (layer *BuildingLayer) GetEntityCoveringPosition(pos GridPosition, footprintLookup func(id int) (int, int)) *BuildingEntity {
	for i := range layer.Entities {
		entity := &layer.Entities[i]
		w, h := footprintLookup(entity.ID)
		if pos.X >= entity.Position.X && pos.X < entity.Position.X+w &&
			pos.Y >= entity.Position.Y && pos.Y < entity.Position.Y+h {
			return entity
		}
	}
	return nil
}

// CheckCollision checks if placing a building at pos with given footprint would collide
func (layer *BuildingLayer) CheckCollision(pos GridPosition, width, height int, footprintLookup func(id int) (int, int)) bool {
	for _, entity := range layer.Entities {
		ew, eh := footprintLookup(entity.ID)
		// Check if rectangles overlap
		if pos.X < entity.Position.X+ew && pos.X+width > entity.Position.X &&
			pos.Y < entity.Position.Y+eh && pos.Y+height > entity.Position.Y {
			return true
		}
	}
	return false
}
