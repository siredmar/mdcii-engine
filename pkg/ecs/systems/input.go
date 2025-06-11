package systems

import (
	"fmt"
	"os"
	"time"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/siredmar/mdcii-engine/pkg/ecs/components"
	"github.com/yohamta/donburi"
	"github.com/yohamta/donburi/filter"
)

var inputQuery = donburi.NewQuery(filter.Contains(components.ControlType))

func InputSystem(world donburi.World) {
	const debounce = time.Millisecond * 250

	inputQuery.Each(world, func(entry *donburi.Entry) {
		ctrl := components.ControlType.Get(entry)
		now := time.Now()

		if now.Sub(ctrl.LastKeyPressTime) >= debounce {
			if ebiten.IsKeyPressed(ebiten.KeyLeft) {
				ctrl.Rotation.Increment()
				fmt.Println("Rotation Incremented:", ctrl.Rotation)
				ctrl.LastKeyPressTime = now
			}
			if ebiten.IsKeyPressed(ebiten.KeyRight) {
				ctrl.Rotation.Decrement()
				fmt.Println("Rotation Decremented:", ctrl.Rotation)
				ctrl.LastKeyPressTime = now
			}
			if ebiten.IsKeyPressed(ebiten.KeyG) {
				ctrl.GridVisible = !ctrl.GridVisible
				ctrl.LastKeyPressTime = now
			}
			if ebiten.IsKeyPressed(ebiten.KeyEscape) {
				os.Exit(0)
			}
		}
	})
}
