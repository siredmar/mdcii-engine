package sprites

import "github.com/hajimehoshi/ebiten/v2"

// Sprite is a lightweight wrapper for legacy terrain tile rendering.
type Sprite struct {
	Image  *ebiten.Image
	Height int
}

// Sprites is a minimal container used by older world/tiles code paths.
type Sprites struct {
	Sprites map[int]Sprite
}

// NewSprites constructs an empty sprite set.
func NewSprites() *Sprites {
	return &Sprites{Sprites: make(map[int]Sprite)}
}
