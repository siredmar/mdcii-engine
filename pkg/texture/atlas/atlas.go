package atlas

import (
	"encoding/json"
	"fmt"
	"image"
	"image/color"
	"image/draw"
	"image/png"
	"math/rand"
	"os"
	"path/filepath"
	"strconv"
	"strings"

	"github.com/lucasb-eyer/go-colorful"
)

// TextureAtlas represents a texture atlas containing multiple images
type TextureAtlas struct {
	Image                *image.RGBA          `json:"-"`
	ImageWithRects       *image.RGBA          `json:"-"`
	AtlasMeta            AtlasMeta            `json:"AtlasMeta"`
	ImagesMeta           map[string]ImageMeta `json:"ImageMeta"`
	OptionSkipFileEnding bool
	OptionKeyToLower     bool
	OptionKeyToUpper     bool
	packer               *MaxRectsPacker
	colors               []color.Color
}

type AtlasMeta struct {
	Width  int `json:"width"`
	Height int `json:"height"`
}

// ImageMeta contains metadata for an image in the atlas
type ImageMeta struct {
	X      int `json:"x"`
	Y      int `json:"y"`
	Width  int `json:"width"`
	Height int `json:"height"`
}

type TextureAtlasOption func(*TextureAtlas)

func WithSkipFileEnding() TextureAtlasOption {
	return func(h *TextureAtlas) {
		h.OptionSkipFileEnding = true
	}
}

func WithKeyToLower() TextureAtlasOption {
	return func(h *TextureAtlas) {
		h.OptionKeyToLower = true
	}
}

func WithKeyToUpper() TextureAtlasOption {
	return func(h *TextureAtlas) {
		h.OptionKeyToUpper = true
	}
}

// HLine draws a horizontal line
func HLine(img *image.RGBA, col color.Color, x1, y, x2 int) {
	for ; x1 <= x2; x1++ {
		img.Set(x1, y, col)
	}
}

// VLine draws a veritcal line
func VLine(img *image.RGBA, col color.Color, x, y1, y2 int) {
	for ; y1 <= y2; y1++ {
		img.Set(x, y1, col)
	}
}

// Rect draws a rectangle utilizing HLine() and VLine()
func Rectangle(img *image.RGBA, col color.Color, x1, y1, x2, y2 int) {
	HLine(img, col, x1, y1, x2)
	HLine(img, col, x1, y2, x2)
	VLine(img, col, x1, y1, y2)
	VLine(img, col, x2, y1, y2)
}

// CreateTextureAtlas creates a texture atlas from a list of image filenames
func CreateTextureAtlas(filenames []string, atlasWidth, atlasHeight int, opts ...TextureAtlasOption) (*TextureAtlas, error) {

	colorTable := make([]color.Color, 100)
	for i := range colorTable {
		// Create a new color in HSL space and convert it to RGB.
		c := colorful.Hsv(float64(i)*360.0/100.0, 1.0, 1.0)
		colorTable[i] = c
	}

	atlas := &TextureAtlas{
		Image:          image.NewRGBA(image.Rect(0, 0, atlasWidth, atlasHeight)),
		ImageWithRects: image.NewRGBA(image.Rect(0, 0, atlasWidth, atlasHeight)),
		ImagesMeta:     make(map[string]ImageMeta),
		AtlasMeta: AtlasMeta{
			Width:  atlasWidth,
			Height: atlasHeight,
		},
		packer: NewMaxRectsPacker(atlasWidth, atlasHeight),
		colors: colorTable,
	}

	// Loop through each option
	for _, opt := range opts {
		opt(atlas)
	}

	for i, filename := range filenames {
		if i == 172 {
			fmt.Println("here")
		}
		key := filename
		if atlas.OptionSkipFileEnding {
			key = strings.TrimSuffix(key, filepath.Ext(key))
			key = filepath.Base(key)
		}
		if atlas.OptionKeyToLower && atlas.OptionKeyToUpper {
			return nil, fmt.Errorf("cannot use both WithKeyToLower and WithKeyToUpper")
		}
		if atlas.OptionKeyToLower {
			key = strings.ToLower(key)
		}
		if atlas.OptionKeyToUpper {
			key = strings.ToUpper(key)
		}
		fmt.Println("key", key)
		img, err := loadImage(filename)
		if err != nil {
			return nil, err
		}

		rect, err := atlas.packer.Pack(img.Bounds().Dx(), img.Bounds().Dy())
		if err != nil {
			fmt.Println("resizing")
			// Double the size of the atlas
			// atlas.AtlasMeta.Width +=200
			inc := 400
			atlas.AtlasMeta.Height += inc
			new := image.NewRGBA(image.Rect(0, 0, atlas.AtlasMeta.Width, atlas.AtlasMeta.Height))
			draw.Draw(new, new.Bounds(), atlas.Image, atlas.Image.Bounds().Min, draw.Over)
			// atlas.exportPNG("tmp.png")
			atlas.Image = new
			// atlas.exportPNG("tmp.png")
			// Create a new packer with the updated size
			// atlas.packer = NewMaxRectsPacker(atlas.AtlasMeta.Width, atlas.AtlasMeta.Height)
			atlas.packer.Height += inc

			// find last FreeRect that is the most bottom one
			bottomFreeRectIndex := func() int {
				ymax := 0
				index := 0
				for i, r := range atlas.packer.FreeRects {
					if r.Y > ymax {
						ymax = r.Y
						index = i
					}
				}
				return index
			}()
			// remove the last FreeRect
			atlas.packer.FreeRects[bottomFreeRectIndex].Height += inc
			// atlas.packer.FreeRects = append(atlas.packer.FreeRects[:bottomFreeRectIndex], atlas.packer.FreeRects[bottomFreeRectIndex+1:]...)
			// atlas.packer.FreeRects = append(atlas.packer.FreeRects, Rect{
			// 	X: 0,
			// 	// Y:      atlas.packer.FreeRects[len(atlas.packer.FreeRects)-1].Y + atlas.packer.FreeRects[len(atlas.packer.FreeRects)-1].Height,
			// 	Y: func() int {
			// 		ymax := 0
			// 		for _, r := range atlas.packer.FreeRects {
			// 			if r.Y > ymax {
			// 				ymax = r.Y
			// 			}
			// 		}
			// 		return ymax
			// 	}(),
			// 	Width:  atlas.AtlasMeta.Width,
			// 	Height: inc,
			// })

			// Try packing the image again
			rect, err = atlas.packer.Pack(img.Bounds().Dx(), img.Bounds().Dy())
			if err != nil {
				return nil, err
				// fmt.Println("Error:", err)
			}
		}

		dstRect := image.Rect(rect.X, rect.Y, rect.X+rect.Width, rect.Y+rect.Height)

		draw.Draw(atlas.Image, dstRect, img, img.Bounds().Min, draw.Over)

		if i > 171 {
			keyInt, err := strconv.Atoi(key)
			if err != nil {
				return nil, fmt.Errorf("key %s is not an integer", key)
			}
			atlas.ImageWithRects = image.NewRGBA(atlas.Image.Rect)
			draw.Draw(atlas.ImageWithRects, atlas.Image.Rect, atlas.Image, atlas.Image.Bounds().Min, draw.Over)
			atlas.drawFreeRects(atlas.ImageWithRects)
			err = atlas.exportPNG(fmt.Sprintf("atlas_%dx%d_%04d_%04d.png", atlas.AtlasMeta.Width, atlas.AtlasMeta.Height, i, keyInt), atlas.ImageWithRects)
			if err != nil {
				return nil, err
			}
		}
		atlas.ImagesMeta[key] = ImageMeta{
			X:      rect.X,
			Y:      rect.Y,
			Width:  rect.Width,
			Height: rect.Height,
		}
	}

	return atlas, nil
}

func (a *TextureAtlas) drawFreeRects(img *image.RGBA) {
	// loop over all FreeRects and draw a rectangle for each
	oldRandomvalue := 0
	for _, r := range a.packer.FreeRects {
		newRandomValue := rand.Intn(99)
		// loop as long as the newRandomValue is far enough away from the old one (+-20)
		for newRandomValue > oldRandomvalue-30 && newRandomValue < oldRandomvalue+30 {
			newRandomValue = rand.Intn(99)
		}
		oldRandomvalue = newRandomValue
		Rectangle(img, a.colors[newRandomValue], r.X, r.Y, r.X+r.Width, r.Y+r.Height)
	}
}

func (a *TextureAtlas) exportPNG(filename string, img *image.RGBA) error {
	exportPNGFile, err := os.Create(filename)
	if err != nil {
		return err
	}
	defer exportPNGFile.Close()

	if err := png.Encode(exportPNGFile, img); err != nil {
		return err
	}
	return nil
}

// Export saves the texture atlas and its metadata as a JSON file and a PNG file
func (a *TextureAtlas) Export(outputJSONFilename, outputPNGFilename string) error {
	err := a.exportPNG(outputPNGFilename, a.Image)
	if err != nil {
		return err
	}

	// Save metadata to JSON file
	exportJSONFile, err := os.Create(outputJSONFilename)
	if err != nil {
		return err
	}
	defer exportJSONFile.Close()

	j, err := json.MarshalIndent(a, "", "  ")
	if err != nil {
		return err
	}

	exportJSONFile.Write(j)

	// encoder := json.NewEncoder(exportJSONFile)
	// encoder.SetIndent("", "  ")

	// if err := encoder.Encode(atlas); err != nil {
	// 	return err
	// }

	return nil
}

// loadImage loads an image from the specified file path
func loadImage(filename string) (image.Image, error) {
	file, err := os.Open(filename)
	if err != nil {
		return nil, fmt.Errorf("failed to open image file: %v", err)
	}
	defer file.Close()

	img, _, err := image.Decode(file)
	if err != nil {
		return nil, fmt.Errorf("failed to decode image: %v", err)
	}

	return img, nil
}
