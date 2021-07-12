package chunks

import (
	"bytes"

	"github.com/HewlettPackard/structex"
)

type Deers struct {
	deers []deerData
}

type deerData struct {
	IslandNumber uint `bitfield:"8"`
	PosX         uint `bitfield:"8"`
	PosY         uint `bitfield:"8"`
	TimeCnt      uint `bitfield:"32"`
	Reserved     int  `bitfield:"8"`
}

func NewDeer(c *Chunk) (*Deers, error) {
	deers := &Deers{}
	deerSizeU, err := structex.Size(deerData{})
	deerSize := int(deerSizeU)
	if err != nil {
		return nil, err
	}
	for i := 0; i < c.Length; i = i + deerSize {
		d := &deerData{}
		deerData := c.Data[i : i+deerSize]
		if err := structex.Decode(bytes.NewReader(deerData), d); err != nil {
			return &Deers{}, err
		}
		deers.deers = append(deers.deers, *d)
	}

	return deers, nil
}
