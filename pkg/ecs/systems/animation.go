package systems

import (
	"github.com/siredmar/mdcii-engine/pkg/ecs/components"
	"github.com/siredmar/mdcii-engine/pkg/world/rotation"

	animations "github.com/siredmar/mdcii-engine/pkg/texture/animations"

	"github.com/yohamta/donburi"
	"github.com/yohamta/donburi/filter"
)

// Define a query using filter.LayoutFilter
var animationQuery = donburi.NewQuery(
	filter.Contains(components.BuildingType, components.AnimationType, components.TileType),
)

func AnimationSystem(world donburi.World, ani *animations.Animations, deltaTime float64) {
	var rot rotation.Rotation

	controlQuery := donburi.NewQuery(filter.Contains(components.ControlType))
	controlQuery.Each(world, func(entry *donburi.Entry) {
		ctrl := components.ControlType.Get(entry)
		rot = ctrl.Rotation
	})
	globalRotation := rot

	animationQuery.Each(world, func(entry *donburi.Entry) {
		// Access the Animation and Tile components
		building := components.BuildingType.Get(entry)
		if building.BuildingID == -1 {
			return
		}
		animation := components.AnimationType.Get(entry)
		if animation.Reset {
			animation.CurrentTime = 0
			animation.Reset = false
		}
		rotation := building.Rotation.Add(globalRotation)
		animationData := ani.GetAnimation(building.BuildingID, rotation)
		images := animationData.Images
		tile := components.TileType.Get(entry)

		// Always set both image and metadata for the tile
		tile.Image = nil
		tile.Metadata = nil
		if len(images) > 0 {
			tile.Image = images[0].Sprite
			tile.Metadata = &images[0].Metadata
		}
		if animation.Count > 1 && animation.Running && len(images) > 0 {
			animation.CurrentTime += deltaTime
			frameDuration := animation.Duration / float64(len(images))
			frameIndex := int(animation.CurrentTime / frameDuration)
			if animation.Loop {
				frameIndex %= len(images)
			} else if frameIndex >= len(images) {
				frameIndex = len(images) - 1
			}
			animation.CurrentFrame = frameIndex
			tile.Image = images[frameIndex].Sprite
			tile.Metadata = &images[frameIndex].Metadata
			return
		}
	})
}
