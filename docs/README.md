# Documentation

This folder is intended to document all those things for which doxygen comments are unsuitable.

## Config

The engine reads configuration from `config.yaml` (default location: `~/.mdcii/config.yaml`, fallback: `./config.yaml`).

Example:

```yaml
mouse_pan_factor: 0.67
renderer: "2d"
```

Renderer options:

- `2d` (default): Ebiten isometric renderer
- `3d`: Raylib billboard renderer for improved z-ordering (requires `-tags=raylib,rgfw` build)
