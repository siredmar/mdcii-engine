package systems

import (
	"github.com/siredmar/mdcii-engine/pkg/ecs/components"

	animations "github.com/siredmar/mdcii-engine/pkg/texture/animations"

	"github.com/yohamta/donburi"
	"github.com/yohamta/donburi/filter"
)

// Define a query using filter.LayoutFilter
var animationQuery = donburi.NewQuery(
	filter.Contains(components.AnimationType, components.TileType),
)

func AnimationSystem(world donburi.World, animations *animations.Animations, deltaTime float64) {
	animationQuery.Each(world, func(entry *donburi.Entry) {
		// Access the Animation and Tile components
		animation := components.AnimationType.Get(entry)
		if animation.Reset {
			animation.CurrentTime = 0
			animation.Reset = false
		}

		if animation.Running {
			tile := components.TileType.Get(entry)

			// Get animation frames from the atlas
			frames := animations.GetAnimation(animation.BuildingID, tile.Rotation).Frames
			// Update animation time
			animation.CurrentTime += deltaTime
			frameDuration := animation.Duration / float64(len(frames))
			frameIndex := int(animation.CurrentTime / frameDuration)

			if animation.Loop {
				frameIndex %= len(frames)
			} else if frameIndex >= len(frames) {
				frameIndex = len(frames) - 1
			}

			animation.CurrentFrame = frameIndex
			// Update the tile's current image
			tile.Image = frames[frameIndex]
		}
	})
}
