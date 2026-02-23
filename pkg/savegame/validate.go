package savegame

import (
	"fmt"
)

// Validate checks that the savegame has valid data
func (s *Savegame) Validate() error {
	if s.Version == "" {
		return fmt.Errorf("version is required")
	}

	if s.World.Width <= 0 || s.World.Height <= 0 {
		return fmt.Errorf("world dimensions must be positive: got %dx%d", s.World.Width, s.World.Height)
	}

	for i, island := range s.World.Islands {
		if err := island.Validate(); err != nil {
			return fmt.Errorf("island %d: %w", i, err)
		}
	}

	return nil
}

// Validate checks that the island has valid data
func (island *Island) Validate() error {
	if island.Dimensions.Width <= 0 || island.Dimensions.Height <= 0 {
		return fmt.Errorf("island dimensions must be positive: got %dx%d",
			island.Dimensions.Width, island.Dimensions.Height)
	}

	if island.Climate != ClimateNorth && island.Climate != ClimateSouth {
		return fmt.Errorf("invalid climate: %s", island.Climate)
	}

	return nil
}
