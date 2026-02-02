# AI Testing Guide

This document provides instructions for AI assistants to run the game, take screenshots, and analyze rendering output.

## Game Data Location

The Anno 1602 game files are located at:
```
/home/armin/spiele/anno1602
```

## Running the Game Viewer

### Basic Run
```bash
cd /home/armin/dev/mdcii-engine
go run cmd/island_ecs/main.go -p /home/armin/spiele/anno1602
```

### With Screenshot (Automated Testing)
```bash
go run cmd/island_ecs/main.go -p /home/armin/spiele/anno1602 \
  --screenshot /tmp/test-screenshot.png \
  --screenshotAfterFrames 120 \
  --exitAfterScreenshot
```

**Parameters:**
- `--screenshot <path>`: Path to save the screenshot PNG
- `--screenshotAfterFrames <n>`: Wait N frames before taking screenshot (default: 60)
- `--exitAfterScreenshot`: Exit immediately after screenshot (default: true)

### Other Useful Flags
- `-r <0-3>`: Initial world rotation (0=0°, 1=90°, 2=180°, 3=270°)
- `-s <path>`: Savegame file path relative to game path (default: `SAVEGAME/lastgame.gam`)

## Screenshot Analysis Workflow

### 1. Take a Screenshot
```bash
go run cmd/island_ecs/main.go -p /home/armin/spiele/anno1602 \
  --screenshot /tmp/debug.png --screenshotAfterFrames 180
```

Use 120-180 frames to ensure:
- Atlas is loaded
- Animations have started
- FPS counter has stabilized

### 2. View the Screenshot
Use the `view` tool to analyze the screenshot:
```
view /tmp/debug.png
```

### 3. Check the HUD Overlay
The screenshot includes a HUD in the upper-left corner showing:
- **FPS**: Current frame rate (green=good, yellow=ok, red=bad)
- **Tiles**: `N drawn, M culled` - rendering statistics
- **Camera**: Current camera position in tile coordinates
- **Zoom**: Current zoom level as percentage
- **Mouse Tile**: Tile position under cursor
- **Island**: Index of hovered island (-1 if none)

### 4. Iterate on Fixes
```bash
# Make code changes, then re-test
go run cmd/island_ecs/main.go -p /home/armin/spiele/anno1602 \
  --screenshot /tmp/debug2.png --screenshotAfterFrames 180
```

## Common Issues to Check

### Rendering Gaps
- **Sea gaps**: Look for white/black lines between sea tiles
- **Island edges**: Check beach/coast transitions are smooth
- **Building overlap**: Verify depth sorting is correct

### Performance
- **FPS < 30**: Major performance issue
- **FPS 30-55**: Acceptable but could be improved
- **FPS > 55**: Good performance
- **Tiles drawn**: High counts (>15k) at low zoom indicate culling issues

### Camera/Zoom
- **Pop-in**: Tiles appearing suddenly at screen edges when panning
- **Culling**: Tiles disappearing when they should be visible
- **Zoom center**: Zoom should center on screen middle

## Regenerating the Texture Atlas

If sprites look wrong or atlas is corrupted:
```bash
rm -rf /tmp/atlas
go run cmd/island_ecs/main.go -p /home/armin/spiele/anno1602 \
  --screenshot /tmp/atlas-test.png --screenshotAfterFrames 180
```

The atlas will be regenerated on next run (takes ~30-60 seconds).

## Interactive Testing

For manual testing without screenshots:
```bash
go run cmd/island_ecs/main.go -p /home/armin/spiele/anno1602
```

**Controls:**
| Key | Action |
|-----|--------|
| WASD / Arrows | Pan camera |
| Q / E | Rotate world |
| Mouse wheel | Zoom in/out |
| Right-click drag | Pan camera |
| G | Toggle debug grid |
| O | Toggle origin markers |
| Escape | Exit |

## Build Verification

Always verify build before testing:
```bash
go build -mod=mod ./cmd/island_ecs/...
```
