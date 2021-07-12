package bsh

import (
	"fmt"
	"image"
	"image/color"
	"image/png"
	"io/ioutil"
	"os"

	"github.com/siredmar/mdcii-engine/pkg/palette"
)

type BshPng struct {
	Bsh     []Image
	Palette *palette.Palette
}

func NewPng(file string, palette *palette.Palette) (*BshPng, error) {
	b, err := ioutil.ReadFile(file)
	if err != nil {
		return &BshPng{}, err
	}
	bsh, err := ParseBsh(b)
	if err != nil {
		return &BshPng{}, err
	}
	return &BshPng{
		Bsh:     bsh,
		Palette: palette,
	}, nil
}

func (bsh *BshPng) Convert(index int, outputName string) error {
	if bsh.Bsh[index].Header.Type != 1 {
		return fmt.Errorf("Image is not of type 1")
	}
	img := image.NewNRGBA(image.Rect(0, 0, bsh.Bsh[index].Header.Width, bsh.Bsh[index].Header.Height))

	// Preload image with transparent pixels
	for y := 0; y < bsh.Bsh[index].Header.Height; y++ {
		for x := 0; x < bsh.Bsh[index].Header.Width; x++ {
			img.Set(x, y, color.NRGBA{
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
				img.Set(x, y, color.NRGBA{
					R: col.R,
					G: col.G,
					B: col.B,
					A: 255,
				})
			}
			x++
		}
	}

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
