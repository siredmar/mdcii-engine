# AI Agent Quick Reference - mdcii-engine

> **Main documentation:** `.github/copilot-instructions.md`  
> **Detailed docs:** `.github/docs/` (8 topic files, ~3000 lines total)

Quick links:
- `.github/copilot-instructions.md`
- `.github/docs/ai-testing.md`
- `.github/docs/bsh-format.md`
- `.github/docs/chunk-format.md`
- `.github/docs/cod-format.md`
- `.github/docs/ecs-architecture.md`
- `.github/docs/islands.md`
- `.github/docs/rendering.md`
- `.github/docs/rotation.md`
- `.github/docs/texture-atlas.md`

---

## 30-Second Overview

**Project:** Go reimplementation of Anno 1602 game engine  
**Stack:** Go + Ebiten (rendering) + Donburi (ECS)  
**Purpose:** Load original game files → parse → render isometric islands

---

## Essential Commands

```bash
make                                              # Build + test
go run cmd/island_ecs/main.go -p /path/to/anno    # Run viewer
go test ./...                                     # All tests
rm -rf /tmp/atlas                                 # Force atlas rebuild
```

---

## Core Formulas

```go
screenX = (gridX - gridY) * 32   // Isometric projection
screenY = (gridX + gridY) * 16
drawX   = screenX - pivotX       // Sprite positioning
```

---

## Critical Facts

| Concept | Rule |
|---------|------|
| Origin point | Top of isometric diamond |
| Building anchor | Back corner (grid 0,0) |
| Empty sentinel | `0xFFFF` |
| Layer order | Sea(0) → Ground(2) → Roads(3) → Forest/Buildings(4) |
| Atlas cache | `/tmp/atlas/` (version 3) |

---

## File → Documentation Map

| Working on... | Read these docs |
|---------------|-----------------|
| Sprites/BSH | bsh-format.md, texture-atlas.md |
| Building defs | cod-format.md |
| Savegames | chunk-format.md, islands.md |
| Rendering | rendering.md, ecs-architecture.md |
| Rotation | rotation.md |

---

## Key Source Files

| Area | Files |
|------|-------|
| Entry point | `cmd/island_ecs/cmd/root.go` |
| Rendering | `pkg/ecs/systems/renderer.go` |
| ECS components | `pkg/ecs/components/*.go` |
| Island parsing | `pkg/chunks/island5.go`, `islandhouse.go` |
| Atlas generation | `pkg/texture/atlas/atlas.go` |
| Building defs | `pkg/cod/buildings/building.go` |

---

## Viewer Controls

| Key | Action |
|-----|--------|
| WASD/Arrows | Pan |
| Q/E | Rotate world |
| G | Debug grid |
| O | Origin markers |
| Esc | Exit |

---

## Verified Facts (from previous sessions)

1. Atlas anchor = back corner (b.X, b.Y), not front
2. Pivot = anchor - cropBounds.Min
3. Layer merge uses sentinel 0xFFFF for "use base"
4. Control.Rotation = world rotation, applied in Draw()

---

*For complete documentation, see `.github/copilot-instructions.md`*
