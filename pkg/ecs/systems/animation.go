package systems

import (
	"fmt"

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
		anim := ani.GetAnimation(building.BuildingID, rotation)
		if anim == nil {
			pos := components.PositionType.Get(entry)
			fmt.Printf("[ANIM_NIL] BuildingID=%d pos=(%d,%d) bldRot=%d global=%d final=%d\n",
				building.BuildingID, int(pos.X), int(pos.Y), building.Rotation, globalRotation, rotation)
			return
		}
		frames := anim.Frames
		metas := anim.FrameMeta
		if len(frames) == 0 || frames[0] == nil {
			pos := components.PositionType.Get(entry)
			fmt.Printf("[ANIM_EMPTY] BuildingID=%d pos=(%d,%d) bldRot=%d global=%d final=%d frames=%d\n",
				building.BuildingID, int(pos.X), int(pos.Y), building.Rotation, globalRotation, rotation, len(frames))
			return
		}
		
		// DEBUG: Log specific building for investigation
		if building.BuildingID == 2702 && globalRotation == 2 {
			pos := components.PositionType.Get(entry)
			fmt.Printf("[ANIM] BuildingID=2702 pos=(%d,%d) bldRot=%d global=%d final=%d frames=%d\n",
				int(pos.X), int(pos.Y), building.Rotation, globalRotation, rotation, len(frames))
		}
		
		tile := components.TileType.Get(entry)
		// if tile.Occupation {
		// 	tile.Image = grid1
		// }
		if animation.Count > 1 {
			if animation.Running {
				// Get animation frames from the atlas
				// Update animation time
				animation.CurrentTime += deltaTime
				// fmt.Println("animation.CurrentTime", animation.CurrentTime)
				// fmt.Println("len(frames)", len(frames))
				// fmt.Println("animation.Duration", animation.Duration)
				frameDuration := animation.Duration / float64(len(frames))
				// fmt.Println("frameDuration", frameDuration)
				frameIndex := int(animation.CurrentTime / frameDuration)
				// fmt.Println("frameIndex", frameIndex)
				if animation.Loop {
					frameIndex %= len(frames)
				} else if frameIndex >= len(frames) {
					frameIndex = len(frames) - 1
				}

				animation.CurrentFrame = frameIndex
				// Update the tile's current image
				tile.Image = frames[frameIndex]
				if frameIndex < len(metas) {
					tile.PivotX = metas[frameIndex].PivotX
					tile.PivotY = metas[frameIndex].PivotY
				}
			}
			return
		}
		tile.Image = frames[0]
		if len(metas) > 0 {
			tile.PivotX = metas[0].PivotX
			tile.PivotY = metas[0].PivotY
		}
	})
}
