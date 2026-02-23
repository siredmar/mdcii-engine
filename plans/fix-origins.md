# fix-origin execution (2026-01-26)

Goal: fix incorrect building placement and draw ordering in `cmd/island_ecs` by using atlas-provided pivots/anchors and stable crop bounds.

## Workplan
- [x] Pivot metadata in atlas JSON (`PivotX/PivotY`)
- [x] Stable cropping across animation frames (union crop)
- [x] Runtime applies pivots when selecting frames (AnimationSystem sets `tile.PivotX/Y`)
- [x] Renderer positions sprites using `draw = origin - pivot`
- [x] Renderer depth ordering based on layer + tile origin (no sprite-height hack)
- [x] Add screenshot capture mode to `cmd/island_ecs` and verify output under `/tmp`
- [x] Fix island chunk grid construction (was appending to a pre-sized slice)
- [x] Fix building footprint placement bug (`y+dy`)
- [x] Use per-tile rotation when initializing animations (`CreateIslandFromChunk`)
- [x] Prevent roads/forest drawing under building footprints (occupancy masking)
- [x] Fix rotation math in renderer (RotatePosition expects local island coords)
- [x] Cleanup: revert accidental `cmd/animations_ecs` renderer signature change (or update that tool properly).
- [x] Fix empty tile sentinel handling (`0xFFFF`) in chunk finalize/merge.
- [x] Render infinite sea background outside island bounds.
- [x] Fix atlas rotation stride (use COD `Rotate` field, not footprint tile count)
- [x] Fix layer merge order (overlay/buildings take priority over base terrain)
- [x] Add runtime sprite rotation for tiles with Rotate=0 but non-zero orientation
- [x] Compare coastline/terrain artifacts vs `assets/reference.png` - now matches closely
- [x] Fix black artifacts on coastal tiles (add sea underlay for BurnCorner, Surf, Beach, etc.)
- [x] Fix black artifacts on slope/cliff tiles (add ground underlay for Slope, SlopeCorner, Rock, etc.)
- [x] Fix coastline rendering (layer-first sorting, skip only deep sea tiles)
- [x] Render shallow water tiles around coastline (1202, 1203, 1204, 1209)
- [x] Fix multi-tile building pivot points (anchor at back corner, not front)
- [x] Fix tree/building depth interleaving (shared layer priority 4)
- [x] Fix forest tiles from base layer during merge (skip if building/road overlay)
- [x] Fix proper building/forest layer handling
- [ ] Fix water tiles appearing inside island in rotated views (rotation != 0)

## Current Status

Rotation 0 renders correctly and matches reference image closely.
Rotations 1, 2, 3 have water tiles appearing inside the island where they shouldn't.

### Remaining Issue: Water Tiles in Rotated Views

When the island is rotated (--rotation 1, 2, or 3), water tiles appear inside the 
island mass. This happens because:
1. Water tiles exist at edge positions in original island data
2. When visually rotated, these edge positions appear "inside" the visual island shape
3. No land tile exists at those rotated screen positions to cover them

### Proposed Fix

1. Draw sea background everywhere (not masked by island bounds)
2. Add rotation-aware ground fill that draws grass at ALL island positions
3. Skip water tile rendering entirely for rotated views (rotation != 0)
4. Ground tiles and other terrain will render on top of ground fill

Files to modify:
- `pkg/ecs/systems/renderer.go` - add drawGroundFill function with rotation support

## Commits made
- `f3d94e4` fix(atlas): correct rotation stride
- `b1a481a` fix(island): correct layer merge and add runtime sprite rotation  
- `2220239` fix(render): add sea underlay for coastal transition tiles
- `5545931` fix(render): add ground underlay for slope/cliff tiles
- `7bf8423` fix(render): unified tile sorting for proper underlay rendering
- `9b634c5` fix(render): exclude sea entities, use background only
- `f0e8f51` fix(render): layer-first sorting and skip plain sea tiles
- `09fbd50` fix(render): render shallow water tiles around coastline
- `bf8192c` fix(atlas): use back corner anchor for building pivot alignment
- `bead950` fix(render): interleave trees and buildings for proper depth sorting
- `ab8570d` fix(island): skip forest tiles from base layer during merge
- `253cae5` fix(island): proper building/forest layer handling
