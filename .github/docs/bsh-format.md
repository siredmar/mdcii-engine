# BSH File Format

BSH (Bitmap SHape) files are the sprite archives used by Anno 1602. They contain indexed-color images compressed with a simple RLE (Run-Length Encoding) scheme.

## File Structure

### Header (20 bytes)

| Offset | Size | Field | Description |
|--------|------|-------|-------------|
| 0x00 | 16 | Id | Magic identifier string (null-terminated, e.g., "BSH\0") |
| 0x10 | 4 | PayloadLength | Total length of payload after header |
| 0x14 | 4 | NumberOfImages | Number of images × 4 (divide by 4 to get actual count) |

```go
// pkg/bsh/reader.go
type bshHeaderRaw struct {
    Id             [16]byte
    PayloadLength  uint32
    NumberOfImages uint32  // Actual count = NumberOfImages / 4
}
```

### Image Offset Table

Immediately after the header, there's a table of 4-byte little-endian offsets, one per image. Each offset is relative to the end of the main header (add 20 to get absolute file position).

```go
// Calculate absolute offset for image i:
offset := toInt(data[20 + i*4 : 20 + i*4 + 4]) + 20
```

### Per-Image Header (16 bytes)

| Offset | Size | Field | Description |
|--------|------|-------|-------------|
| 0x00 | 4 | Width | Image width in pixels |
| 0x04 | 4 | Height | Image height in pixels |
| 0x08 | 4 | Type | Image type (must be 1 for valid images) |
| 0x0C | 4 | Length | Total length including header and pixel data |

```go
// pkg/bsh/reader.go
type imageHeaderRaw struct {
    Width  uint `bitfield:"32"`
    Height uint `bitfield:"32"`
    Type   uint `bitfield:"32"`
    Length uint `bitfield:"32"`
}
```

## RLE Pixel Data Decoding

The pixel data follows the image header and uses a custom RLE scheme optimized for sprites with transparency.

### Algorithm

```go
// pkg/bsh/png.go - Parse() method
x := 0
y := 0
v := 0  // data pointer

for {
    data := int(bsh.Bsh[index].Data[v])
    v++

    if data == 0xFF {
        // End of image marker
        break
    }

    if data == 0xFE {
        // End of row - move to next line
        x = 0
        y++
        continue
    }

    // Skip 'data' transparent pixels
    x += data
    
    // Read pixel count for this run
    pixels := int(bsh.Bsh[index].Data[v])
    v++
    
    // Read 'pixels' palette indices
    for i := 0; i < pixels; i++ {
        paletteIndex := int(bsh.Bsh[index].Data[v])
        v++
        
        // Look up color from palette
        col := palette.Colors[paletteIndex]
        img.Set(x, y, color.RGBA{col.R, col.G, col.B, 255})
        x++
    }
}
```

### Control Bytes Summary

| Byte Value | Meaning |
|------------|---------|
| `0xFF` | End of image |
| `0xFE` | End of current row, advance to next line |
| `0x00-0xFD` | Skip N transparent pixels, then read next byte for opaque pixel count |

### Example Decoding

For a 4x3 image with a horizontal line in the middle:

```
Data: 00 04 [A B C D] FE    <- Row 0: skip 0, draw 4 pixels (ABCD), end row
      00 04 [E F G H] FE    <- Row 1: skip 0, draw 4 pixels (EFGH), end row  
      00 04 [I J K L] FF    <- Row 2: skip 0, draw 4 pixels (IJKL), end image
```

For a sprite with transparency (only center pixel drawn):

```
Data: FE                    <- Row 0: empty (transparent)
      01 01 [X] FE          <- Row 1: skip 1, draw 1 pixel (X), end row
      FF                    <- End image (row 2 is implicitly transparent)
```

## Palette

BSH images use indexed colors from Anno's palette. The default palette is defined in `pkg/palette/palette.go` with 256 RGB colors.

```go
// Usage
bshPng.Palette = &palette.DefaultPalette
col := bshPng.Palette.Colors[paletteIndex]  // Returns RGBA
```

## Usage in Codebase

### Loading BSH Files

```go
import "github.com/siredmar/mdcii-engine/pkg/bsh"

// Load and convert all images to PNG format in memory
pngs, err := bsh.NewPng(
    bsh.WithFile("path/to/stadtfld.bsh"),
    bsh.WithConvertAll(),
)

// Access individual images by string index
img := pngs.Images["0"]    // First image
img := pngs.Images["123"]  // Image at index 123
```

### Options

```go
// Convert specific indices only
bsh.WithConvertIndex(42)

// Export to disk as PNG files
bsh.WithExportToPNG("/output/dir", "sprite")  // Creates sprite-0.png, sprite-1.png, ...

// Use custom palette
bsh.WithPalette(myPalette)

// Load from raw bytes instead of file
bsh.WithBshChunk(rawBytes)
```

## Key BSH Files in Anno 1602

| File | Content | Approximate Count |
|------|---------|-------------------|
| `GFX/STADTFLD.BSH` | Building and terrain sprites | ~2000+ |
| `GFX/MAEUSE.BSH` | UI elements, cursors | Variable |
| `GFX/SHIP.BSH` | Ship sprites | Variable |
| `GFX/TRAEGER.BSH` | Unit/figure sprites | Variable |

## Implementation Notes

1. **Image Type Check**: Only type 1 images are supported. Other types may exist but aren't decoded.

2. **Pre-initialized Transparency**: The output image is initialized with fully transparent pixels (RGBA 0,0,0,0) before decoding.

3. **String Index Keys**: Images are stored in a map with string keys for historical reasons. Use `strconv.Itoa(index)` to convert.

4. **Memory Efficiency**: All images are held in memory after `NewPng()`. For large BSH files, consider loading only needed indices.

## Related Files

- `pkg/bsh/reader.go` - Binary parsing of BSH structure
- `pkg/bsh/png.go` - RLE decoding and PNG conversion
- `pkg/palette/palette.go` - Color palette definitions
