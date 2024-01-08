package chunks

import (
	"bytes"
	"fmt"

	"github.com/HewlettPackard/structex"
	"github.com/siredmar/mdcii-engine/pkg/files"
)

type island5Data struct {
	IslandNumber    int                `bitfield:"8"`          // byte0
	Width           int                `bitfield:"8"`          // byte1
	Height          int                `bitfield:"8"`          // byte2
	Strtduerrflg    int                `bitfield:"1"`          // byte3.0
	Nofixflg        int                `bitfield:"1"`          // byte3.1
	Vulkanflg       int                `bitfield:"1"`          // byte3.2
	Reserved0       int                `bitfield:"5,reserved"` // byte3.3 - 7
	Posx            int                `bitfield:"16"`         // byte 4,5
	Posy            int                `bitfield:"16"`         // byte 6,7
	Hirschreviercnt int                `bitfield:"16"`         // byte 8,9
	Speedcnt        int                `bitfield:"16"`         // byte 10,11
	Stadtplayernr   [11]uint8          // byte 12 - 22
	Vulkancnt       int                `bitfield:"8"` // byte 23
	Schatzflg       int                `bitfield:"8"` // byte 24
	Rohstanz        int                `bitfield:"8"` // byte 25
	Eisencnt        int                `bitfield:"8"` // byte 26
	Playerflags     int                `bitfield:"8"` // byte 27
	Eisenberg       [4]OreMountainData // byte 28 + 4 * 8
	Vulkanberg      [4]OreMountainData // byte 60 + 4 * 8
	Fertility       Fertility          `bitfield:"32"`          // byte 92 - 95
	FileNumber      int                `bitfield:"16"`          // byte 96,97
	Size            IslandSize         `bitfield:"16"`          // byte 98,99
	Climate         IslandClimate      `bitfield:"8"`           // byte 100
	ModifiedFlag    IslandModified     `bitfield:"8"`           // byte 101
	Duerrproz       int                `bitfield:"8"`           // byte 102
	Rotier          int                `bitfield:"8"`           // byte 103
	Seeplayerflags  int                `bitfield:"32"`          // byte 104 - 107
	Duerrcnt        int                `bitfield:"32"`          // byte 108 - 111
	Reserved1       int                `bitfield:"32,reserved"` // byte 112 - 115
}

type layers struct {
	top         *IslandHouse
	bottom      *IslandHouse
	final       []*IslandHouse
	islandHouse []*IslandHouse
}

type Island5 struct {
	island5Data
	layers layers
}

func NewIsland5(c *Chunk) (*Island5, error) {
	islandSizeU, err := structex.Size(island5Data{})
	islandSize := int(islandSizeU)
	if err != nil {
		return nil, err
	}
	i := &island5Data{}
	i5Data := c.Data[0:islandSize]
	if err := structex.Decode(bytes.NewReader(i5Data), i); err != nil {
		return &Island5{}, err
	}
	return &Island5{
		island5Data: *i,
		layers: layers{
			top:         nil,
			bottom:      nil,
			final:       make([]*IslandHouse, 0),
			islandHouse: make([]*IslandHouse, 0),
		},
	}, nil
}

func (i *Island5) AddIslandHouse(house IslandHouse) {
	i.layers.islandHouse = append(i.layers.islandHouse, &house)
}

func (i *Island5) IslandFileName() string {
	return fmt.Sprintf("%s/%s%02d.scp", IslandClimateMap[i.Climate], IslandSizeMap[i.Size], i.FileNumber)
}

func (i *Island5) SetIslandNumber(number int) {
	i.IslandNumber = number
}

func (i *Island5) SetPosition(x, y int) {
	i.Posx = x
	i.Posy = y
}

func (i *Island5) SetIslandFile(fileNumber int) {
	i.FileNumber = fileNumber
}

func (i *Island5) Finalize() error {
	// island is unmodified, load bottom islandHouse from island file
	if i.ModifiedFlag == ModifiedFalse && len(i.layers.islandHouse) <= 1 {
		// load the unmodified bottom layer from the island .scp file
		islandFile := i.IslandFileName()
		f := files.Instance()
		path, err := f.FindPathForFile(islandFile)
		if err != nil {
			return err
		}
		chunksFromFile, err := NewChunksFromFile(path)
		if err != nil {
			return err
		}
		foundIslandHouseChunk, err := FindChunkById(chunksFromFile, "INSELHAUS")
		if err != nil {
			return err
		}
		inselHouse, err := NewIslandHouse(foundIslandHouseChunk, IslandDimensions{i.Width, i.Height})
		if err != nil {
			return err
		}
		i.layers.islandHouse = append(i.layers.islandHouse, inselHouse)

		if len(i.layers.islandHouse) == 2 {
			i.layers.final = append(i.layers.final, i.layers.islandHouse[0])
			i.layers.final = append(i.layers.final, i.layers.islandHouse[1])
		}

		// there is only one islandHouse chunk present, this is the bot layer
		if len(i.layers.islandHouse) == 1 {
			i.layers.final = append(i.layers.final, i.layers.islandHouse[0])
			// create empty top
			empty := NewEmptyIslandHouse(IslandDimensions{i.Width, i.Height})
			i.layers.final = append(i.layers.final, empty)
		}
	} else {
		// the island is modified, first chunk is bottom
		i.layers.final = append(i.layers.final, i.layers.islandHouse[0])
		// a possible second chunk is top
		if len(i.layers.islandHouse) == 2 {
			i.layers.final = append(i.layers.final, i.layers.islandHouse[1])
		} else {
			// create empty top
			empty := NewEmptyIslandHouse(IslandDimensions{i.Width, i.Height})
			i.layers.final = append(i.layers.final, empty)
		}
	}
	i.layers.bottom = i.layers.final[0]
	i.layers.top = i.layers.final[1]
	return nil
}
