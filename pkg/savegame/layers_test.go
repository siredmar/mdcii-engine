package savegame

import (
	"testing"
)

func TestAddTile(t *testing.T) {
	layers := &IslandLayers{}

	tile := Tile{ID: 100, Position: GridPosition{X: 5, Y: 5}, Rotation: 1}

	if err := layers.AddTile("ground", tile); err != nil {
		t.Fatalf("failed to add tile: %v", err)
	}

	if layers.Ground == nil {
		t.Fatal("ground layer is nil after adding tile")
	}

	if len(layers.Ground.Tiles) != 1 {
		t.Fatalf("expected 1 tile, got %d", len(layers.Ground.Tiles))
	}

	if layers.Ground.Tiles[0].ID != 100 {
		t.Errorf("tile ID mismatch: expected 100, got %d", layers.Ground.Tiles[0].ID)
	}
}

func TestAddTileAllLayers(t *testing.T) {
	layers := &IslandLayers{}

	testCases := []struct {
		layerName string
		tileID    int
	}{
		{"sea", 1201},
		{"ground", 0},
		{"roads", 1100},
		{"forest", 1300},
	}

	for _, tc := range testCases {
		tile := Tile{ID: tc.tileID, Position: GridPosition{X: 0, Y: 0}, Rotation: 0}
		if err := layers.AddTile(tc.layerName, tile); err != nil {
			t.Errorf("failed to add tile to %s: %v", tc.layerName, err)
		}
	}

	// Verify all layers have tiles
	if layers.Sea == nil || len(layers.Sea.Tiles) != 1 {
		t.Error("sea layer incorrect")
	}
	if layers.Ground == nil || len(layers.Ground.Tiles) != 1 {
		t.Error("ground layer incorrect")
	}
	if layers.Roads == nil || len(layers.Roads.Tiles) != 1 {
		t.Error("roads layer incorrect")
	}
	if layers.Forest == nil || len(layers.Forest.Tiles) != 1 {
		t.Error("forest layer incorrect")
	}
}

func TestAddTileInvalidLayer(t *testing.T) {
	layers := &IslandLayers{}
	tile := Tile{ID: 0, Position: GridPosition{X: 0, Y: 0}, Rotation: 0}

	if err := layers.AddTile("invalid", tile); err == nil {
		t.Error("expected error for invalid layer name")
	}
}

func TestRemoveTile(t *testing.T) {
	layers := &IslandLayers{
		Ground: &TileLayer{
			Tiles: []Tile{
				{ID: 0, Position: GridPosition{X: 5, Y: 5}, Rotation: 0},
				{ID: 1, Position: GridPosition{X: 6, Y: 6}, Rotation: 0},
			},
		},
	}

	if err := layers.RemoveTile("ground", GridPosition{X: 5, Y: 5}); err != nil {
		t.Fatalf("failed to remove tile: %v", err)
	}

	if len(layers.Ground.Tiles) != 1 {
		t.Fatalf("expected 1 tile remaining, got %d", len(layers.Ground.Tiles))
	}

	if layers.Ground.Tiles[0].ID != 1 {
		t.Errorf("wrong tile remaining: expected ID 1, got %d", layers.Ground.Tiles[0].ID)
	}
}

func TestGetTileAt(t *testing.T) {
	layers := &IslandLayers{
		Ground: &TileLayer{
			Tiles: []Tile{
				{ID: 100, Position: GridPosition{X: 5, Y: 5}, Rotation: 2},
			},
		},
	}

	tile := layers.GetTileAt("ground", GridPosition{X: 5, Y: 5})
	if tile == nil {
		t.Fatal("expected to find tile")
	}

	if tile.ID != 100 {
		t.Errorf("tile ID mismatch: expected 100, got %d", tile.ID)
	}

	if tile.Rotation != 2 {
		t.Errorf("tile rotation mismatch: expected 2, got %d", tile.Rotation)
	}

	// Test not found
	notFound := layers.GetTileAt("ground", GridPosition{X: 10, Y: 10})
	if notFound != nil {
		t.Error("expected nil for non-existent tile")
	}
}

func TestBuildingLayerAddEntity(t *testing.T) {
	layer := &BuildingLayer{}

	entity := BuildingEntity{
		ID:        1804,
		Position:  GridPosition{X: 10, Y: 10},
		Rotation:  1,
		Owner:     1,
		City:      0,
		Animation: Animation{Frame: 0, Running: true, Loop: true},
	}

	layer.AddEntity(entity)

	if len(layer.Entities) != 1 {
		t.Fatalf("expected 1 entity, got %d", len(layer.Entities))
	}

	if layer.Entities[0].ID != 1804 {
		t.Errorf("entity ID mismatch: expected 1804, got %d", layer.Entities[0].ID)
	}
}

func TestBuildingLayerRemoveEntity(t *testing.T) {
	layer := &BuildingLayer{
		Entities: []BuildingEntity{
			{ID: 100, Position: GridPosition{X: 5, Y: 5}},
			{ID: 200, Position: GridPosition{X: 10, Y: 10}},
		},
	}

	removed := layer.RemoveEntity(GridPosition{X: 5, Y: 5})
	if !removed {
		t.Error("expected entity to be removed")
	}

	if len(layer.Entities) != 1 {
		t.Fatalf("expected 1 entity remaining, got %d", len(layer.Entities))
	}

	if layer.Entities[0].ID != 200 {
		t.Errorf("wrong entity remaining: expected ID 200, got %d", layer.Entities[0].ID)
	}

	// Try to remove non-existent
	removed = layer.RemoveEntity(GridPosition{X: 99, Y: 99})
	if removed {
		t.Error("expected false for non-existent entity")
	}
}

func TestBuildingLayerGetEntityAt(t *testing.T) {
	layer := &BuildingLayer{
		Entities: []BuildingEntity{
			{ID: 100, Position: GridPosition{X: 5, Y: 5}},
		},
	}

	entity := layer.GetEntityAt(GridPosition{X: 5, Y: 5})
	if entity == nil {
		t.Fatal("expected to find entity")
	}

	if entity.ID != 100 {
		t.Errorf("entity ID mismatch: expected 100, got %d", entity.ID)
	}

	notFound := layer.GetEntityAt(GridPosition{X: 10, Y: 10})
	if notFound != nil {
		t.Error("expected nil for non-existent entity")
	}
}

func TestBuildingLayerGetEntityCoveringPosition(t *testing.T) {
	layer := &BuildingLayer{
		Entities: []BuildingEntity{
			{ID: 100, Position: GridPosition{X: 5, Y: 5}}, // 3x2 building
		},
	}

	footprintLookup := func(id int) (int, int) {
		if id == 100 {
			return 3, 2 // Width=3, Height=2
		}
		return 1, 1
	}

	// Test position inside footprint
	entity := layer.GetEntityCoveringPosition(GridPosition{X: 6, Y: 6}, footprintLookup)
	if entity == nil {
		t.Fatal("expected to find entity covering position")
	}

	// Test position outside footprint
	outside := layer.GetEntityCoveringPosition(GridPosition{X: 10, Y: 10}, footprintLookup)
	if outside != nil {
		t.Error("expected nil for position outside footprint")
	}
}

func TestBuildingLayerCheckCollision(t *testing.T) {
	layer := &BuildingLayer{
		Entities: []BuildingEntity{
			{ID: 100, Position: GridPosition{X: 5, Y: 5}}, // 3x2 building
		},
	}

	footprintLookup := func(id int) (int, int) {
		if id == 100 {
			return 3, 2
		}
		return 1, 1
	}

	// Overlapping placement
	collision := layer.CheckCollision(GridPosition{X: 6, Y: 5}, 2, 2, footprintLookup)
	if !collision {
		t.Error("expected collision")
	}

	// Non-overlapping placement
	noCollision := layer.CheckCollision(GridPosition{X: 10, Y: 10}, 2, 2, footprintLookup)
	if noCollision {
		t.Error("expected no collision")
	}
}
