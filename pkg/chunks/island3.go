package chunks

import (
	"bytes"

	"github.com/HewlettPackard/structex"
)

type Island3 struct {
	IslandNumber int       `bitfield:"8"`  // ID for this island (per game)
	Width        int       `bitfield:"8"`  // width
	Height       int       `bitfield:"8"`  // height
	Unknown0     int       `bitfield:"8"`  // TODO: unknown
	Posx         int       `bitfield:"16"` // position of island x
	Posy         int       `bitfield:"16"` // position of island y
	Unknown1     int       `bitfield:"16"` // TODO: unknown
	Unknown2     int       `bitfield:"16"` // TODO: unknown
	Unknown3     [28]uint8 // TODO: unknown
}

func NewIsland3(c *Chunk) (*Island3, error) {
	islandSizeU, err := structex.Size(Island3{})
	islandSize := int(islandSizeU)
	if err != nil {
		return nil, err
	}
	i := &Island3{}
	Island3Data := c.Data[0:islandSize]
	if err := structex.Decode(bytes.NewReader(Island3Data), i); err != nil {
		return &Island3{}, err
	}
	return i, nil
}

func (i *Island3) ToIsland5() *Island5 {
	return &Island5{
		island5Data: island5Data{
			IslandNumber:    i.IslandNumber,
			Width:           i.Width,
			Height:          i.Height,
			Strtduerrflg:    0,
			Nofixflg:        0,
			Vulkanflg:       0,
			Reserved0:       0,
			Posx:            i.Posx,
			Posy:            i.Posy,
			Hirschreviercnt: 0,
			Speedcnt:        0,
			Stadtplayernr:   [11]uint8{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
			Vulkancnt:       0,
			Schatzflg:       0,
			Rohstanz:        0,
			Eisencnt:        0,
			Playerflags:     0,
			Eisenberg:       [4]OreMountainData{},
			Vulkanberg:      [4]OreMountainData{},
			Fertility:       Random,
			FileNumber:      0,
			Size:            LittleIsland,
			Climate:         0,
			ModifiedFlag:    0,
			Duerrproz:       0,
			Rotier:          0,
			Seeplayerflags:  0,
			Duerrcnt:        0,
			Reserved1:       0,
		},
	}
}
