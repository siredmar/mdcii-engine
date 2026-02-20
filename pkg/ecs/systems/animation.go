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
		animationDef := ani.GetAnimation(building.BuildingID, rotation)
		frames := animationDef.Frames
		metas := animationDef.FrameMeta
		tile := components.TileType.Get(entry)

		// Sync count/duration from the animation definition so changes to
		// BuildingID or rotation are picked up automatically.
		animation.Count = animationDef.Steps
		if animationDef.FrameDuration > 0 {
			animation.Duration = float64(animationDef.FrameDuration) / 1000.0 * float64(animation.Count)
		}

		if animation.Count > 1 {
			if animation.Running {
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
			}
			// Apply current frame image (works for both auto-play and manual stepping)
			frameIndex := animation.CurrentFrame
			if frameIndex >= len(frames) {
				frameIndex = len(frames) - 1
			}
			tile.Image = frames[frameIndex]
			if frameIndex < len(metas) {
				meta := metas[frameIndex]
				tile.PivotX = meta.PivotX
				tile.PivotY = meta.PivotY
				tile.AtlasIndex = meta.PNGIndex
				tile.SrcX = meta.X
				tile.SrcY = meta.Y
				tile.SrcW = meta.Width
				tile.SrcH = meta.Height
			}
			return
		}
		tile.Image = frames[0]
		if len(metas) > 0 {
			meta := metas[0]
			tile.PivotX = meta.PivotX
			tile.PivotY = meta.PivotY
			tile.AtlasIndex = meta.PNGIndex
			tile.SrcX = meta.X
			tile.SrcY = meta.Y
			tile.SrcW = meta.Width
			tile.SrcH = meta.Height
		}
	})
}
