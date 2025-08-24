package ecs

import (
	"github.com/yohamta/donburi/ecs"
)

// System bundle registration helper
func Register(dispatcher *ecs.ECS) { // simplified, systems now use *ecs.ECS signature
	dispatcher.AddSystem(InputSystem)
	dispatcher.AddSystem(AnimationSystem)
	dispatcher.AddSystem(TransformSystem)
	dispatcher.AddSystem(SortSystem)
}
