package ecs

import (
	rl "github.com/gen2brain/raylib-go/raylib"
	"github.com/yohamta/donburi/ecs"
)

var (
	deltaTime      float32
	globalRotDirty bool
)

func SetDeltaTime(dt float32) { deltaTime = dt }

func InputSystem(e *ecs.ECS) { // updated signature
	_, island := IslandEntity(e.World)
	if island == nil {
		return
	}
	if rl.IsKeyPressed(rl.KeyQ) {
		island.GlobalRot = (island.GlobalRot + 3) & 3
		globalRotDirty = true
	}
	if rl.IsKeyPressed(rl.KeyE) {
		island.GlobalRot = (island.GlobalRot + 1) & 3
		globalRotDirty = true
	}
}
