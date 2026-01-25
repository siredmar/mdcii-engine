# Agents

This repository is an open-source remake of **Anno 1602** (isometric city builder). The codebase is in the middle of a larger transition: the original C++ engine is being ported to Go, and most of the *data side* is already in place.

This document is for contributors (human or AI “agents”) to quickly understand:

- what is already implemented,
- what the current bottleneck is,
- how the rendering pipeline is intended to work,
- where to focus next to get a savegame rendering correctly (with animations).

## Current State

The project has a working foundation for reading original Anno 1602 assets and savegames:

- COD parsing is implemented (`pkg/cod`, building metadata via `pkg/cod/buildings`).
- BSH decoding and PNG conversion is implemented (`pkg/bsh`).
- Chunk readers for savegame data exist (`pkg/chunks`).
- Savegame parsing exists (`pkg/gam`).

The current focus is the renderer engine.

## Goal

Primary milestone:

- Render a real savegame island correctly (tile layers + buildings) with correct ordering and correct animations.

“Correct ordering” matters because Anno 1602 is isometric and sprites can be larger than 1×1 tiles and overlap across tiles.

## Why Rendering Is Hard Here

Earlier approaches used Ebiten (2D rendering). The main pain point was **z-depth / draw ordering**:

- Isometric scenes cannot be trivially sorted by tile (x,y) when sprites differ in footprint (1×1, 2×2, …), height, and when they overlap across tiles.
- Handling all overlap cases with a pure 2D sort quickly becomes complex and brittle.

The current approach is to use a real 3D pipeline with an isometric camera and let the GPU depth buffer resolve ordering.

## Texture Atlas Approach

To simplify texture management and animation frame lookup, the project uses a *prebaked atlas*.

What “atlas” means in this repo:

- For each building ID, the atlas contains all rotations (0/90/180/270) and all animation steps.
- Each atlas entry stores metadata: sheet index, source rectangle, and frame dimensions.
- Atlas sheets are exported as PNG files plus a JSON metadata file.

Relevant code:

- Atlas generator/loader: `pkg/texture/atlas/atlas.go`
- Animation frame mapping: `pkg/texture/animations/animations.go`

## Rendering Architecture (Today)

The current experiment uses raylib (via `raylib-go`) and a small ECS built on donburi.

- Main runnable that loads a savegame + atlas: `cmd/island_ecs/cmd/root.go`
- 3D renderer utilities (isometric camera + billboards): `pkg/renderer/raylib/renderer.go`

Notes:

- `pkg/renderer/raylib/renderer.go` already contains an orthographic isometric camera setup and a helper to draw an atlas region as a billboard.
- `cmd/island_ecs/cmd/root.go` currently renders with a 2D `DrawTexturePro` placeholder and uses placeholder atlas frames for entities.

## Git History Signals

The commit history shows the evolution clearly:

- 2024-12: “atlas and animations working”, “rendering animations working with ECS” (2D path)
- 2025-06: “rendering multitiles working”, “rotation wip”
- 2025-08: “rendering a dark isometric rectangle”, “port to raylib”, “refactor … for raylib 3d”

In other words: data pipeline is strong; the project stalls at “make 3D depth sorting work while staying pixel-correct and animation-correct”.

## What’s Stalling Right Now

The missing glue is a *coherent render pipeline* that connects:

1. Savegame world entities (tiles/buildings, positions, rotation, animation state)
2. Atlas metadata (sheet index + source rect per frame)
3. A 3D scene representation that produces correct ordering via depth

The last step is where progress slowed.

## What To Do Next (Recommended Path)

These steps are intentionally practical and incremental.

1) Make `cmd/island_ecs` render via raylib 3D

- Switch the render pass to `BeginMode3D` + `DrawBillboardRec` (or equivalent)
- Load atlas sheets into **one or more** `rl.Texture2D` values, then select by frame `PNGIndex`
- Enable depth testing in the 3D pass

2) Replace placeholder frames with real atlas/animation lookup

- Build an `Animations` object from the loaded atlas: `pkg/texture/animations/animations.go`
- For each building entity, pick the correct animation (building id + world rotation)
- Advance frame index over time (fixed timestep or dt accumulator)
- Write the chosen frame’s `(PNGIndex, Src)` into the render component

3) Establish world-to-3D mapping (tile coords → 3D coords)

- Define a consistent mapping for isometric tiles (grid X/Y) into 3D (X/Z plane)
- Place sprites as billboards at `(tileX + 0.5, y, tileZ + 0.5)`
- Decide what “y” means (height) for now; start with y=0 and add height later

4) Validate correctness with a minimal scene

- Render only a small subset: one island chunk or a fixed N×N tile region
- Render only top-layer buildings first
- Then add terrain/water tiles and other layers after depth works reliably

## Practical Constraints / Gotchas

- **Pixel correctness**: an orthographic camera is required; avoid perspective.
- **Texture filtering**: use point sampling to avoid blur; clamp wrap to avoid sampling bleed.
- **Atlas bleeding**: when sampling sub-rectangles, inset the UV rectangle slightly (the renderer helper already does this).
- **Transparency + depth**: billboards are alpha-blended; ordering can still matter for transparent pixels.
  Start by ensuring depth test is enabled and verify visually. If needed later, consider alpha cutout for some layers or split passes.

## Entry Points For New Contributors

If you are new to the repo, these are the best places to start reading:

- `cmd/island_ecs/cmd/root.go` (loads cod/bsh/gam, builds atlas, bootstraps ECS)
- `pkg/gam/parser.go` and `pkg/chunks/*` (how savegame data becomes an island model)
- `pkg/texture/atlas/atlas.go` + `pkg/texture/animations/animations.go` (frame metadata)
- `pkg/renderer/raylib/renderer.go` (current 3D camera + billboard drawing)

## Working Agreements For Agents

- Keep progress measurable: prefer small steps that render *something correct*.
- Preserve determinism: avoid “magic numbers” unless documented and centralized.
- Keep the asset pipeline stable: atlas JSON/PNG format is a contract—version it if you change it.
- When changing rendering, provide a quick way to reproduce (command + expected visual result).

## Repro

The current rendering testbed is:

- `go run ./cmd/island_ecs --path <path-to-anno-install>`

It expects a savegame at `assets/savegames/lastgame.gam` and builds/loads atlas sheets under `./atlas`.
