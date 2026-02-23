package savegame

import (
	"encoding/json"
	"fmt"
	"os"
	"time"
)

// New creates a new empty Savegame with default values
func New() *Savegame {
	return &Savegame{
		Version: CurrentVersion,
		Metadata: Metadata{
			CreatedAt:  time.Now(),
			GameTick:   0,
			Extensions: make(map[string]any),
		},
		Meta: SavegameMeta{
			Camera: Camera{
				X:        0,
				Y:        0,
				Zoom:     1.0,
				Rotation: 0,
			},
		},
		World: World{
			Width:   500,
			Height:  350,
			Islands: []Island{},
		},
	}
}

// Save writes the savegame to a JSON file
func (s *Savegame) Save(path string) error {
	data, err := s.ToJSON()
	if err != nil {
		return fmt.Errorf("failed to serialize savegame: %w", err)
	}

	if err := os.WriteFile(path, data, 0644); err != nil {
		return fmt.Errorf("failed to write savegame file: %w", err)
	}

	return nil
}

// Load reads a savegame from a JSON file
func Load(path string) (*Savegame, error) {
	data, err := os.ReadFile(path)
	if err != nil {
		return nil, fmt.Errorf("failed to read savegame file: %w", err)
	}

	return FromJSON(data)
}

// ToJSON serializes the savegame to JSON bytes
func (s *Savegame) ToJSON() ([]byte, error) {
	return json.MarshalIndent(s, "", "  ")
}

// FromJSON deserializes a savegame from JSON bytes
func FromJSON(data []byte) (*Savegame, error) {
	var s Savegame
	if err := json.Unmarshal(data, &s); err != nil {
		return nil, fmt.Errorf("failed to parse savegame JSON: %w", err)
	}

	if err := s.Validate(); err != nil {
		return nil, fmt.Errorf("savegame validation failed: %w", err)
	}

	return &s, nil
}
