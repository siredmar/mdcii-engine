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
		// rotation := (building.Rotation + globalRotation) % 4
		rotation := building.Rotation.Add(globalRotation)
		// fmt.Println("AnimationSystem: building.BuildingID", building.BuildingID, "rotation", rotation, "globalRotation", globalRotation)
		frames := ani.GetAnimation(building.BuildingID, rotation).Frames
		tile := components.TileType.Get(entry)
		if animation.Count > 1 {
			if animation.Running {
				animation.CurrentTime += deltaTime
				frameDuration := animation.Duration / float64(len(frames))
				frameIndex := int(animation.CurrentTime / frameDuration)
				if animation.Loop {
					frameIndex %= len(frames)
				} else if frameIndex >= len(frames) {
					frameIndex = len(frames) - 1
				}

				animation.CurrentFrame = frameIndex
				tile.PNGIndex = frames[frameIndex].PNGIndex
				tile.Src = frames[frameIndex].Src
			}
			return
		}
		tile.PNGIndex = frames[0].PNGIndex
		tile.Src = frames[0].Src
	})
}
