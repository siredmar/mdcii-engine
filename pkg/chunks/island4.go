package chunks

import (
	"bytes"

	"github.com/HewlettPackard/structex"
)

type Island4 struct {
	FileNumber      int `bitfield:"8"`
	Width           int `bitfield:"8"`
	Height          int `bitfield:"8"`
	Strtduerrflg    int `bitfield:"1"`
	Nofixflg        int `bitfield:"1"`
	Vulkanflg       int `bitfield:"1"`
	Reserved0       int `bitfield:"5,reserved"`
	Posx            int `bitfield:"16"`
	Posy            int `bitfield:"16"`
	Hirschreviercnt int `bitfield:"16"`
	Speedcnt        int `bitfield:"16"`
	Stadtplayernr   [11]uint8
	Vulkancnt       int `bitfield:"8"`
	Schatzflg       int `bitfield:"8"`
	Rohstanz        int `bitfield:"8"`
	Eisencnt        int `bitfield:"8"`
	Playerflags     int `bitfield:"8"`
	Eisenberg       [4]OreMountainData
	Vulkanberg      [4]OreMountainData
	Fertility       Fertility      `bitfield:"32"`
	Reserved2       int            `bitfield:"16"` // always 0xFFFF
	Size            IslandSize     `bitfield:"16"`
	Climate         IslandClimate  `bitfield:"8"`
	ModifiedFlag    IslandModified `bitfield:"8"`
	Duerrproz       int            `bitfield:"8"`
	Rotier          int            `bitfield:"8"`
	Seeplayerflags  int            `bitfield:"32"`
	Duerrcnt        int            `bitfield:"32"`
	Reserved1       int            `bitfield:"32,reserved"`
}

func NewIsland4(c *Chunk) (*Island4, error) {
	islandSizeU, err := structex.Size(Island4{})
	islandSize := int(islandSizeU)
	if err != nil {
		return nil, err
	}
	i := &Island4{}
	island4Data := c.Data[0:islandSize]
	if err := structex.Decode(bytes.NewReader(island4Data), i); err != nil {
		return &Island4{}, err
	}
	return i, nil
}

func (i *Island4) ToIsland5() *Island5 {
	return &Island5{
		island5Data: island5Data{
			IslandNumber:    0,
			Width:           i.Width,
			Height:          i.Height,
			Strtduerrflg:    i.Strtduerrflg,
			Nofixflg:        i.Nofixflg,
			Vulkanflg:       i.Vulkanflg,
			Reserved0:       i.Reserved0,
			Posx:            i.Posx,
			Posy:            i.Posy,
			Hirschreviercnt: i.Hirschreviercnt,
			Speedcnt:        i.Speedcnt,
			Stadtplayernr:   i.Stadtplayernr,
			Vulkancnt:       i.Vulkancnt,
			Schatzflg:       i.Schatzflg,
			Rohstanz:        i.Rohstanz,
			Eisencnt:        i.Eisencnt,
			Playerflags:     i.Playerflags,
			Eisenberg:       i.Eisenberg,
			Vulkanberg:      i.Vulkanberg,
			Fertility:       i.Fertility,
			FileNumber:      i.FileNumber,
			Size:            i.Size,
			Climate:         i.Climate,
			ModifiedFlag:    ModifiedTrue,
			Duerrproz:       i.Duerrproz,
			Rotier:          i.Rotier,
			Seeplayerflags:  i.Seeplayerflags,
			Duerrcnt:        i.Duerrcnt,
			Reserved1:       i.Reserved1,
		},
	}
}
