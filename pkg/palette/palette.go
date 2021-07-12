package palette

import (
	"fmt"
	"io/ioutil"
)

const PALETTE_FILE = "stadtfld.col"

type Rgb struct {
	R uint8
	G uint8
	B uint8
}

type Palette struct {
	Colors           []Rgb
	TransparentIndex int
	TransparentColor Rgb
}

func NewPalette(file string) (*Palette, error) {
	f, err := ioutil.ReadFile(file)
	if err != nil {
		return nil, err
	}
	return ParsePalette(f)
}

func ParsePalette(data []byte) (*Palette, error) {
	colors := []Rgb{}
	// 20 is the chunk definition int bytes to jump over
	transparentIndex := -1
	colorIndex := 0
	for i := 20; i < len(data)-4; i = i + 4 {
		c := Rgb{
			R: data[i],
			G: data[i+1],
			B: data[i+2],
		}
		colors = append(colors, c)
		// Transparent color hard coded to magenta
		if c.R == 255 && c.G == 0 && c.B == 255 {
			transparentIndex = colorIndex
		}
		colorIndex++
	}
	return &Palette{
		Colors:           colors,
		TransparentIndex: transparentIndex,
		TransparentColor: Rgb{
			R: 255,
			G: 0,
			B: 255,
		},
	}, nil
}

func (p *Palette) GetColor(index int) (*Rgb, error) {
	if index >= len(p.Colors) {
		return nil, fmt.Errorf("cannot get color index. Out of boundaries")
	}
	return &p.Colors[index], nil
}
