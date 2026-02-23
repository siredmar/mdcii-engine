package savegame

import "time"

// Savegame represents the complete savegame structure
type Savegame struct {
	Version  string       `json:"version"`
	Metadata Metadata     `json:"metadata"`
	Meta     SavegameMeta `json:"meta"`
	World    World        `json:"world"`
}

// Metadata contains savegame metadata and extension points
type Metadata struct {
	CreatedAt  time.Time      `json:"created_at"`
	GameTick   int64          `json:"game_tick"`
	Extensions map[string]any `json:"extensions,omitempty"`
}

// SavegameMeta contains general savegame information
type SavegameMeta struct {
	Name        string `json:"name,omitempty"`
	Description string `json:"description,omitempty"`
	PlayTime    int64  `json:"play_time,omitempty"` // Total play time in seconds
	Camera      Camera `json:"camera"`
}

// Camera represents the camera state
type Camera struct {
	X           float64 `json:"x"`
	Y           float64 `json:"y"`
	Zoom        float64 `json:"zoom"`
	Rotation    int     `json:"rotation"`
	Initialized bool    `json:"initialized"` // True if camera position was set by user
}

// World contains global world data and all islands
type World struct {
	Width   int      `json:"width"`
	Height  int      `json:"height"`
	Islands []Island `json:"islands"`
}

// Island represents a single island in the world
type Island struct {
	ID         int          `json:"id"`
	Position   GridPosition `json:"position"`
	Dimensions Dimensions   `json:"dimensions"`
	Climate    Climate      `json:"climate"`
	Fertility  []string     `json:"fertility,omitempty"`
	Layers     IslandLayers `json:"layers"`
}

// GridPosition represents a position in the grid
type GridPosition struct {
	X int `json:"x"`
	Y int `json:"y"`
}

// Dimensions represents width and height
type Dimensions struct {
	Width  int `json:"width"`
	Height int `json:"height"`
}

// Climate represents the island climate type
type Climate string

const (
	ClimateNorth Climate = "north"
	ClimateSouth Climate = "south"
)

// IslandLayers contains all layers for an island
type IslandLayers struct {
	Sea       *TileLayer     `json:"sea,omitempty"`
	Ground    *TileLayer     `json:"ground,omitempty"`
	Roads     *TileLayer     `json:"roads,omitempty"`
	Forest    *TileLayer     `json:"forest,omitempty"`
	Buildings *BuildingLayer `json:"buildings,omitempty"`
}

// TileLayer contains tiles for a single layer
type TileLayer struct {
	Tiles []Tile `json:"tiles"`
}

// Tile represents a single tile in a layer
type Tile struct {
	ID        int          `json:"id"`
	Position  GridPosition `json:"position"`
	Rotation  int          `json:"rotation"`
	Animation *Animation   `json:"animation,omitempty"`
}

// BuildingLayer contains building entities
type BuildingLayer struct {
	Entities []BuildingEntity `json:"entities"`
}

// BuildingEntity represents a building as an entity at its origin position
type BuildingEntity struct {
	ID        int          `json:"id"`
	Position  GridPosition `json:"position"`
	Rotation  int          `json:"rotation"`
	Owner     int          `json:"owner"`
	City      int          `json:"city"`
	Animation Animation    `json:"animation"`
	State     *EntityState `json:"state,omitempty"`
}

// Animation represents the animation state of a tile or building
type Animation struct {
	Frame      int     `json:"frame"`
	TimeOffset float64 `json:"time_offset"`
	Running    bool    `json:"running"`
	Loop       bool    `json:"loop"`
}

// EntityState provides an extension point for future gameplay state
type EntityState struct {
	Custom map[string]any `json:"custom,omitempty"`
}
