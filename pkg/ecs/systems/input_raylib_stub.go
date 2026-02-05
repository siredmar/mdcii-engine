//go:build !raylib

package systems

import "github.com/yohamta/donburi"

func InputSystemRaylib(world donburi.World) {
	InputSystem(world)
}
