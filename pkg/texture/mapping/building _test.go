package mapping

import (
	"fmt"
	"testing"

	"github.com/siredmar/mdcii-engine/pkg/cod/buildings"
	"github.com/stretchr/testify/assert"
)

// Generate generates a texture for the given building
// It takes the buildings dimensions and the buildings GFX as a starting point
// The GFX is found in the texture atlas. It maps the single building tiles to create one big texture
// The texture is then used to render the building on the screen
func TestGenerate(t *testing.T) {
	assert := assert.New(t)
	b := &buildings.Building{
		Gfx:    3064,
		Size:   buildings.BuildingSize{W: 2, H: 2},
		Rotate: 4,
	}
	i, err := Generate(b)
	assert.Nil(err)
	assert.NotNil(i)
	fmt.Println(i)
	// a := &atlas.TextureAtlas{}
	// Generate(b, a)
	Draw(i, 0)
	Draw(i, 1)
	Draw(i, 2)
	Draw(i, 3)
}
