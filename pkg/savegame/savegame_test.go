package savegame

import (
	"encoding/json"
	"os"
	"path/filepath"
	"testing"
	"time"
)

func TestNewSavegame(t *testing.T) {
	s := New()

	if s.Version != CurrentVersion {
		t.Errorf("expected version %s, got %s", CurrentVersion, s.Version)
	}

	if s.World.Width != 500 {
		t.Errorf("expected world width 500, got %d", s.World.Width)
	}

	if s.World.Height != 350 {
		t.Errorf("expected world height 350, got %d", s.World.Height)
	}

	if len(s.World.Islands) != 0 {
		t.Errorf("expected 0 islands, got %d", len(s.World.Islands))
	}
}

func TestSavegameSerializationRoundTrip(t *testing.T) {
	s := &Savegame{
		Version: CurrentVersion,
		Metadata: Metadata{
			CreatedAt:  time.Now().Truncate(time.Second),
			GameTick:   12345,
			Extensions: map[string]any{"test": "value"},
		},
		World: World{
			Width:  500,
			Height: 350,
			Islands: []Island{
				{
					ID:         0,
					Position:   GridPosition{X: 50, Y: 100},
					Dimensions: Dimensions{Width: 33, Height: 30},
					Climate:    ClimateNorth,
					Fertility:  []string{"grain", "tobacco"},
					Layers: IslandLayers{
						Sea: &TileLayer{
							Tiles: []Tile{
								{ID: 1201, Position: GridPosition{X: 0, Y: 0}, Rotation: 0},
							},
						},
						Ground: &TileLayer{
							Tiles: []Tile{
								{ID: 0, Position: GridPosition{X: 5, Y: 5}, Rotation: 0},
							},
						},
						Buildings: &BuildingLayer{
							Entities: []BuildingEntity{
								{
									ID:       1804,
									Position: GridPosition{X: 15, Y: 15},
									Rotation: 0,
									Owner:    1,
									City:     0,
									Animation: Animation{
										Frame:      3,
										TimeOffset: 0.5,
										Running:    true,
										Loop:       true,
									},
								},
							},
						},
					},
				},
			},
		},
	}

	// Serialize
	data, err := s.ToJSON()
	if err != nil {
		t.Fatalf("failed to serialize: %v", err)
	}

	// Deserialize
	loaded, err := FromJSON(data)
	if err != nil {
		t.Fatalf("failed to deserialize: %v", err)
	}

	// Verify
	if loaded.Version != s.Version {
		t.Errorf("version mismatch: expected %s, got %s", s.Version, loaded.Version)
	}

	if loaded.Metadata.GameTick != s.Metadata.GameTick {
		t.Errorf("game tick mismatch: expected %d, got %d", s.Metadata.GameTick, loaded.Metadata.GameTick)
	}

	if len(loaded.World.Islands) != 1 {
		t.Fatalf("expected 1 island, got %d", len(loaded.World.Islands))
	}

	island := loaded.World.Islands[0]
	if island.Climate != ClimateNorth {
		t.Errorf("climate mismatch: expected %s, got %s", ClimateNorth, island.Climate)
	}

	if island.Layers.Buildings == nil {
		t.Fatal("buildings layer is nil")
	}

	if len(island.Layers.Buildings.Entities) != 1 {
		t.Fatalf("expected 1 building entity, got %d", len(island.Layers.Buildings.Entities))
	}

	building := island.Layers.Buildings.Entities[0]
	if building.ID != 1804 {
		t.Errorf("building ID mismatch: expected 1804, got %d", building.ID)
	}

	if building.Animation.Frame != 3 {
		t.Errorf("animation frame mismatch: expected 3, got %d", building.Animation.Frame)
	}
}

func TestSavegameSaveLoad(t *testing.T) {
	tmpDir := t.TempDir()
	filePath := filepath.Join(tmpDir, "test_savegame.json")

	s := New()
	s.World.Islands = append(s.World.Islands, Island{
		ID:         0,
		Position:   GridPosition{X: 10, Y: 20},
		Dimensions: Dimensions{Width: 50, Height: 50},
		Climate:    ClimateSouth,
		Layers:     IslandLayers{},
	})

	// Save
	if err := s.Save(filePath); err != nil {
		t.Fatalf("failed to save: %v", err)
	}

	// Verify file exists
	if _, err := os.Stat(filePath); os.IsNotExist(err) {
		t.Fatal("savegame file was not created")
	}

	// Load
	loaded, err := Load(filePath)
	if err != nil {
		t.Fatalf("failed to load: %v", err)
	}

	if len(loaded.World.Islands) != 1 {
		t.Fatalf("expected 1 island, got %d", len(loaded.World.Islands))
	}

	if loaded.World.Islands[0].Climate != ClimateSouth {
		t.Errorf("climate mismatch: expected %s, got %s", ClimateSouth, loaded.World.Islands[0].Climate)
	}
}

func TestValidation(t *testing.T) {
	// Valid savegame
	s := New()
	if err := s.Validate(); err != nil {
		t.Errorf("expected no error for valid savegame, got: %v", err)
	}

	// Invalid: empty version
	s.Version = ""
	if err := s.Validate(); err == nil {
		t.Error("expected error for empty version")
	}
	s.Version = CurrentVersion

	// Invalid: zero world dimensions
	s.World.Width = 0
	if err := s.Validate(); err == nil {
		t.Error("expected error for zero world width")
	}
	s.World.Width = 500

	// Invalid: bad island climate
	s.World.Islands = append(s.World.Islands, Island{
		ID:         0,
		Position:   GridPosition{X: 0, Y: 0},
		Dimensions: Dimensions{Width: 10, Height: 10},
		Climate:    "invalid",
		Layers:     IslandLayers{},
	})
	if err := s.Validate(); err == nil {
		t.Error("expected error for invalid island climate")
	}
}

func TestJSONFormat(t *testing.T) {
	s := New()
	s.World.Islands = append(s.World.Islands, Island{
		ID:         1,
		Position:   GridPosition{X: 100, Y: 200},
		Dimensions: Dimensions{Width: 40, Height: 40},
		Climate:    ClimateNorth,
		Layers: IslandLayers{
			Ground: &TileLayer{
				Tiles: []Tile{{ID: 0, Position: GridPosition{X: 0, Y: 0}, Rotation: 0}},
			},
		},
	})

	data, err := s.ToJSON()
	if err != nil {
		t.Fatalf("failed to serialize: %v", err)
	}

	// Verify it's valid JSON
	var parsed map[string]any
	if err := json.Unmarshal(data, &parsed); err != nil {
		t.Fatalf("output is not valid JSON: %v", err)
	}

	// Verify expected keys exist
	if _, ok := parsed["version"]; !ok {
		t.Error("missing 'version' key")
	}
	if _, ok := parsed["metadata"]; !ok {
		t.Error("missing 'metadata' key")
	}
	if _, ok := parsed["world"]; !ok {
		t.Error("missing 'world' key")
	}
}
