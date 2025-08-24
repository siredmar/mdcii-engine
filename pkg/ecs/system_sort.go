package ecs

import (
	"sort"
	"sync/atomic"

	"github.com/yohamta/donburi"
	"github.com/yohamta/donburi/ecs"
)

var (
	needResort     int32
	sortedEntities []donburi.Entity
)

func markResortNeeded() { atomic.StoreInt32(&needResort, 1) }

func MarkResortNeededForNewEntity() { markResortNeeded() }

// SortSystem collects renderables and sorts when needed.
func SortSystem(es *ecs.ECS) { // updated signature
	if atomic.LoadInt32(&needResort) == 0 {
		return
	}
	var ents []*donburi.Entry
	qRenderable.Each(es.World, func(e *donburi.Entry) { ents = append(ents, e) })
	sort.SliceStable(ents, func(i, j int) bool {
		ri := Render.Get(ents[i])
		rj := Render.Get(ents[j])
		return ri.SortKey < rj.SortKey
	})
	// store entity ids
	var ordered []donburi.Entity
	for _, e := range ents {
		ordered = append(ordered, e.Entity())
	}
	sortedEntities = ordered
	atomic.StoreInt32(&needResort, 0)
}

// SortedRenderableEntities returns current ordered list; fallback re-scan if empty.
func SortedRenderableEntities(es *ecs.ECS) []donburi.Entity { // updated signature
	if len(sortedEntities) == 0 {
		qRenderable.Each(es.World, func(e *donburi.Entry) { sortedEntities = append(sortedEntities, e.Entity()) })
	}
	return sortedEntities
}
