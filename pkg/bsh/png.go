package bsh

import (
	"fmt"
	"image"
	"image/color"
	"image/png"
	"log"
	"os"
	"strconv"

	"github.com/siredmar/mdcii-engine/pkg/palette"
)

type BshPng struct {
	Bsh             []Image
	Images          map[string]image.Image
	Palette         *palette.Palette
	convertIndex    []int
	outputDirectory string
	outputName      string
	exportToPng     bool
}

type PngOption func(*BshPng)

func WithConvertIndex(index int) PngOption {
	return func(h *BshPng) {
		h.convertIndex = append(h.convertIndex, index)
	}
}

func WithConvertAll() PngOption {
	return func(h *BshPng) {
		for i := 0; i < len(h.Bsh); i++ {
			h.convertIndex = append(h.convertIndex, i)
		}
	}
}

func WithExportToPNG(outputDir string, name string) PngOption {
	return func(h *BshPng) {
		h.exportToPng = true
		h.outputDirectory = outputDir
		if err := os.MkdirAll(outputDir, os.ModePerm); err != nil {
			log.Fatal(err)
		}
		h.outputName = name
	}

}

func WithOutputDir(outputDir string) PngOption {
	return func(h *BshPng) {
		h.outputDirectory = outputDir
		if err := os.MkdirAll(outputDir, os.ModePerm); err != nil {
			log.Fatal(err)
		}
	}
}

func WithOutputName(name string) PngOption {
	return func(h *BshPng) {
		h.outputName = name
	}
}

func NewPng(file string, palette *palette.Palette, opts ...PngOption) (*BshPng, error) {
	b, err := os.ReadFile(file)
	if err != nil {
		return &BshPng{}, err
	}
	bsh, err := ParseBsh(b)
	if err != nil {
		return &BshPng{}, err
	}
	bshPng := &BshPng{
		Bsh:             bsh,
		Images:          map[string]image.Image{},
		Palette:         palette,
		convertIndex:    []int{},
		outputDirectory: ".",
		outputName:      "",
		exportToPng:     false,
	}

	for _, opt := range opts {
		opt(bshPng)
	}
	for _, img := range bshPng.convertIndex {
		err := bshPng.Draw(img)
		if err != nil {
			return &BshPng{}, err
		}
		if bshPng.exportToPng {
			if err := bshPng.Export(img, fmt.Sprintf("%s/%s-%d.png", bshPng.outputDirectory, bshPng.outputName, img)); err != nil {
				return &BshPng{}, err
			}
		}
	}
	return bshPng, nil
}

func (bsh *BshPng) Draw(index int) error {
	if bsh.Bsh[index].Header.Type != 1 {
		return fmt.Errorf("Image is not of type 1")
	}
	img := image.NewRGBA(image.Rect(0, 0, bsh.Bsh[index].Header.Width, bsh.Bsh[index].Header.Height))
	// Preload image with transparent pixels
	for y := 0; y < bsh.Bsh[index].Header.Height; y++ {
		for x := 0; x < bsh.Bsh[index].Header.Width; x++ {
			img.Set(x, y, color.RGBA{
				R: 0,
				G: 0,
				B: 0,
				A: 0,
			})
		}
	}

	x := 0
	y := 0
	v := 0
	for {
		data := int(bsh.Bsh[index].Data[v])
		v++

		if data == 0xFF {
			break
		}

		if data == 0xFE {
			x = 0
			y++
			continue
		}

		x += data
		pixels := int(bsh.Bsh[index].Data[v])
		v++
		for i := 0; i < pixels; i++ {
			data = int(bsh.Bsh[index].Data[v])
			v++
			if data < len(bsh.Palette.Colors) {
				col := bsh.Palette.Colors[data]
				img.Set(x, y, color.RGBA{
					R: col.R,
					G: col.G,
					B: col.B,
					A: 255,
				})
			}
			x++
		}
	}

	bsh.Images[strconv.Itoa(index)] = img
	return nil
}

func (bsh *BshPng) Export(index int, outputName string) error {
	img := bsh.Images[strconv.Itoa(index)]

	f, err := os.Create(outputName)
	if err != nil {
		return err
	}

	if err := png.Encode(f, img); err != nil {
		f.Close()
		return err
	}

	if err := f.Close(); err != nil {
		return err
	}
	return nil
}
