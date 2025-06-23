package rotation

import (
	"testing"
)

func TestRotatePositionAroundAnchor(t *testing.T) {
	tests := []struct {
		mapX, mapY, width, height int
		rotation                  Rotation
		expectedX, expectedY      int
	}{
		{0, 0, 4, 4, DEG0, 0, 0},
		{0, 0, 4, 4, DEG90, 3, 0},
		{0, 0, 4, 4, DEG180, 3, 3},
		{0, 0, 4, 4, DEG270, 0, 3},
		{2, 1, 4, 4, DEG0, 2, 1},
		{2, 1, 4, 4, DEG90, 2, 2},
		{2, 1, 4, 4, DEG180, 1, 2},
		{2, 1, 4, 4, DEG270, 1, 1},
	}

	for _, tt := range tests {
		t.Run("", func(t *testing.T) {
			gotX, gotY := RotatePositionAroundAnchor(tt.mapX, tt.mapY, tt.width, tt.height, tt.rotation)
			if gotX != tt.expectedX || gotY != tt.expectedY {
				t.Errorf("RotatePositionAroundAnchor(%d, %d, %d, %d, %v) = (%d, %d), want (%d, %d)",
					tt.mapX, tt.mapY, tt.width, tt.height, tt.rotation, gotX, gotY, tt.expectedX, tt.expectedY)
			}
		})
	}
}
