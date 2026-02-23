package components

import "github.com/yohamta/donburi"

// World holds global world-level data
type World struct {
	Width   int
	Height  int
	Islands []*donburi.Entry // References to island entities
}

var WorldType = donburi.NewComponentType[World]()
