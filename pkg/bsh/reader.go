package bsh

import (
	"bytes"

	"github.com/HewlettPackard/structex"
)

const BSH_HEADER_LENGTH = 20

type bshHeaderRaw struct {
	Id             [16]byte
	PayloadLength  uint32
	NumberOfImages uint32
}

type bshHeader struct {
	Id             string
	PayloadLength  int
	NumberOfImages int
}

type imageHeaderRaw struct {
	Width  uint `bitfield:"32"`
	Height uint `bitfield:"32"`
	Type   uint `bitfield:"32"`
	Length uint `bitfield:"32"`
}

type ImageHeader struct {
	Width  int
	Height int
	Type   int
	Length int
}

type Image struct {
	Header ImageHeader
	Data   []byte
}

func ParseBsh(data []byte) ([]Image, error) {
	raw := &bshHeaderRaw{}
	if err := structex.Decode(bytes.NewReader(data), raw); err != nil {
		return []Image{}, err
	}
	header := bshHeader{
		Id: func(c []byte) string {
			n := -1
			for i, b := range c {
				if b == 0 {
					break
				}
				n = i
			}
			return string(c[:n+1])
		}(raw.Id[:]),
		PayloadLength:  int(raw.PayloadLength),
		NumberOfImages: int(raw.NumberOfImages) / 4,
	}

	imageOffsets := []int{}
	for i := BSH_HEADER_LENGTH; i < BSH_HEADER_LENGTH+header.NumberOfImages*4; i = i + 4 {
		offset := toInt(data[i:i+4]) + BSH_HEADER_LENGTH
		imageOffsets = append(imageOffsets, offset)
	}
	imageHeaderSizeU, err := structex.Size(imageHeaderRaw{})
	if err != nil {
		return nil, err
	}
	images := []Image{}
	for _, offset := range imageOffsets {
		imageHeaderRaw := &imageHeaderRaw{}
		if err := structex.Decode(bytes.NewReader(data[offset:offset+int(imageHeaderSizeU)]), imageHeaderRaw); err != nil {
			return []Image{}, err
		}
		image := Image{
			Header: ImageHeader{
				Width:  int(imageHeaderRaw.Width),
				Height: int(imageHeaderRaw.Height),
				Type:   int(imageHeaderRaw.Type),
				Length: int(imageHeaderRaw.Length),
			},
			Data: data[offset+int(imageHeaderSizeU) : offset+int(imageHeaderRaw.Length)],
		}
		images = append(images, image)
	}

	return images, nil
}

func toInt(fourBytes []byte) int {
	return int(fourBytes[3])<<24 |
		int(fourBytes[2])<<16 |
		int(fourBytes[1])<<8 |
		int(fourBytes[0])
}
