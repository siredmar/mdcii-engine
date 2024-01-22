package atlas

import (
	"encoding/json"
	"fmt"
	"image"
	"image/draw"
	"image/png"
	"os"
	"path/filepath"
	"strings"
)

// TextureAtlas represents a texture atlas containing multiple images
type TextureAtlas struct {
	Images               []*image.RGBA          `json:"-"`
	AtlasMeta            AtlasMeta              `json:"atlasMeta"`
	ImagesMeta           map[string]ImageMeta   `json:"imageMeta"`
	OptionSkipFileEnding bool                   `json:"-"`
	OptionKeyToLower     bool                   `json:"-"`
	OptionKeyToUpper     bool                   `json:"-"`
	imagesToLoad         map[string]image.Image `json:"-"`
	filesToLoad          []string               `json:"-"`
}

type AtlasMeta struct {
	Width  int    `json:"width"`
	Height int    `json:"height"`
	Name   string `json:"name"`
}

// ImageMeta contains metadata for an image in the atlas
type ImageMeta struct {
	ImageIndex int `json:"image"`
	X          int `json:"x"`
	Y          int `json:"y"`
	Width      int `json:"width"`
	Height     int `json:"height"`
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

func WithName(name string) TextureAtlasOption {
	return func(h *TextureAtlas) {
		h.AtlasMeta.Name = name
	}
}

func WithImages(images map[string]image.Image) TextureAtlasOption {
	return func(h *TextureAtlas) {
		h.imagesToLoad = images
	}
}

func WithFiles(files []string) TextureAtlasOption {
	return func(h *TextureAtlas) {
		h.filesToLoad = files
	}
}

// CreateTextureAtlas creates a texture atlas from a list of image filenames
func CreateTextureAtlas(atlasWidth, atlasHeight int, opts ...TextureAtlasOption) (*TextureAtlas, error) {
	atlas := &TextureAtlas{
		Images:     []*image.RGBA{image.NewRGBA(image.Rect(0, 0, atlasWidth, atlasHeight))},
		ImagesMeta: make(map[string]ImageMeta),
		AtlasMeta: AtlasMeta{
			Width:  atlasWidth,
			Height: atlasHeight,
			Name:   "atlas",
		},
		imagesToLoad: make(map[string]image.Image),
		filesToLoad:  []string{},
	}

	// Loop through each option
	for _, opt := range opts {
		opt(atlas)
	}

	currentAtlasIndex := 0
	for _, filename := range atlas.filesToLoad {
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
		img, err := loadImage(filename)
		if err != nil {
			return nil, err
		}
		atlas.imagesToLoad[key] = img
	}

	packer := NewMaxRectsPacker(atlasWidth, atlasHeight)
	for key, img := range atlas.imagesToLoad {
	rewind:
		rect, err := packer.Pack(img.Bounds().Dx(), img.Bounds().Dy())
		if err != nil {
			currentAtlasIndex++
			atlas.Images = append(atlas.Images, image.NewRGBA(image.Rect(0, 0, atlasWidth, atlasHeight)))
			packer = NewMaxRectsPacker(atlasWidth, atlasHeight)
			goto rewind
		}

		dstRect := image.Rect(rect.X, rect.Y, rect.X+rect.Width, rect.Y+rect.Height)

		draw.Draw(atlas.Images[currentAtlasIndex], dstRect, img, img.Bounds().Min, draw.Over)
		atlas.ImagesMeta[key] = ImageMeta{
			ImageIndex: currentAtlasIndex,
			X:          rect.X,
			Y:          rect.Y,
			Width:      rect.Width,
			Height:     rect.Height,
		}
	}

	return atlas, nil
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
func (a *TextureAtlas) Export(outputDir string) error {
	if _, err := os.Stat(outputDir); os.IsNotExist(err) {
		if err := os.MkdirAll(outputDir, os.ModePerm); err != nil {
			return err
		}
	}

	for i, img := range a.Images {
		err := a.exportPNG(fmt.Sprintf("%s/%s-%04d.png", outputDir, a.AtlasMeta.Name, i), img)
		if err != nil {
			return err
		}
	}

	// Save metadata to JSON file
	exportJSONFile, err := os.Create(fmt.Sprintf("%s/%s.json", outputDir, a.AtlasMeta.Name))
	if err != nil {
		return err
	}
	defer exportJSONFile.Close()

	j, err := json.MarshalIndent(a, "", "  ")
	if err != nil {
		return err
	}

	_, err = exportJSONFile.Write(j)
	if err != nil {
		return err
	}
	return nil
}

// loadImage loads an image from the specified file path
func loadImage(filename string) (image.Image, error) {
	file, err := os.Open(filename)
	if err != nil {
		return nil, fmt.Errorf("failed to open image file: %v", err)
	}
	defer file.Close()

	img, err := png.Decode(file)
	if err != nil {
		return nil, fmt.Errorf("failed to decode image: %v", err)
	}
	// if converted, ok := img.(*image.RGBA); ok {
	// 	return converted, nil
	// }

	// return nil, fmt.Errorf("failed to convert image to RGBA")
	return img, nil
}
