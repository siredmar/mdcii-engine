package chunks

import (
	"testing"

	"github.com/stretchr/testify/assert"
)

func TestIsland5(t *testing.T) {
	assert := assert.New(t)

	data := []byte{
		0x49, 0x4E, 0x53, 0x45, 0x4C, 0x35, 0x00, 0x7D, 0x40, 0x3E, 0x49, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x74, 0x00, 0x00, 0x00, 0x00, 0x1E, 0x1E, 0x00, 0x18, 0x01, 0x5A, 0x00, 0x01, 0x00, 0x03, 0x00,
		0x06, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00,
		0x02, 0x07, 0x09, 0x00, 0x01, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x83, 0x11, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	}
	chunk, err := NewChunk(data)
	assert.Nil(err)
	assert.Equal(chunk.Id, "INSEL5")
	assert.Equal(chunk.Length, 116)
	i, err := NewIsland5(chunk)
	assert.Nil(err)
	assert.Equal(i.Height, 30)
	assert.Equal(i.Width, 30)
	assert.Equal(i.IslandNumber, 0)
	assert.Equal(i.Strtduerrflg, 0)
	assert.Equal(i.Nofixflg, 0)
	assert.Equal(i.Vulkanflg, 0)
	assert.Equal(i.Reserved0, 0)
	assert.Equal(i.Posx, 280)
	assert.Equal(i.Posy, 90)
	assert.Equal(i.Hirschreviercnt, 1)
	assert.Equal(i.Speedcnt, 3)
	assert.Equal(i.Stadtplayernr, [11]uint8{6, 7, 7, 7, 7, 7, 7, 7, 0, 0, 0})
	assert.Equal(i.Vulkancnt, 0)
	assert.Equal(i.Schatzflg, 0)
	assert.Equal(i.Rohstanz, 1)
	assert.Equal(i.Eisencnt, 1)
	assert.Equal(i.Playerflags, 0)
	assert.Equal(i.Eisenberg[0].Ware, 2)
	assert.Equal(i.Eisenberg[0].Posx, 7)
	assert.Equal(i.Eisenberg[0].Posy, 9)
	assert.Equal(i.Eisenberg[0].Playerflags, 0)
	assert.Equal(i.Eisenberg[0].Kind, 1)
	assert.Equal(i.Eisenberg[0].Empty, 0)
	assert.Equal(i.Eisenberg[0].Stock, 2560)
	assert.Equal(i.Eisenberg[1].Ware, 0)
	assert.Equal(i.Eisenberg[1].Posx, 0)
	assert.Equal(i.Eisenberg[1].Posy, 0)
	assert.Equal(i.Eisenberg[1].Playerflags, 0)
	assert.Equal(i.Eisenberg[1].Kind, 0)
	assert.Equal(i.Eisenberg[1].Empty, 0)
	assert.Equal(i.Eisenberg[1].Stock, 0)
	assert.Equal(i.Eisenberg[2].Ware, 0)
	assert.Equal(i.Eisenberg[2].Posx, 0)
	assert.Equal(i.Eisenberg[2].Posy, 0)
	assert.Equal(i.Eisenberg[2].Playerflags, 0)
	assert.Equal(i.Eisenberg[2].Kind, 0)
	assert.Equal(i.Eisenberg[2].Empty, 0)
	assert.Equal(i.Eisenberg[2].Stock, 0)
	assert.Equal(i.Eisenberg[3].Ware, 0)
	assert.Equal(i.Eisenberg[3].Posx, 0)
	assert.Equal(i.Eisenberg[3].Posy, 0)
	assert.Equal(i.Eisenberg[3].Playerflags, 0)
	assert.Equal(i.Eisenberg[3].Kind, 0)
	assert.Equal(i.Eisenberg[3].Empty, 0)
	assert.Equal(i.Eisenberg[3].Stock, 0)
	assert.Equal(i.Vulkanberg[0].Ware, 0)
	assert.Equal(i.Vulkanberg[0].Posx, 0)
	assert.Equal(i.Vulkanberg[0].Posy, 0)
	assert.Equal(i.Vulkanberg[0].Playerflags, 0)
	assert.Equal(i.Vulkanberg[0].Kind, 0)
	assert.Equal(i.Vulkanberg[0].Empty, 0)
	assert.Equal(i.Vulkanberg[0].Stock, 0)
	assert.Equal(i.Vulkanberg[1].Ware, 0)
	assert.Equal(i.Vulkanberg[1].Posx, 0)
	assert.Equal(i.Vulkanberg[1].Posy, 0)
	assert.Equal(i.Vulkanberg[1].Playerflags, 0)
	assert.Equal(i.Vulkanberg[1].Kind, 0)
	assert.Equal(i.Vulkanberg[1].Empty, 0)
	assert.Equal(i.Vulkanberg[1].Stock, 0)
	assert.Equal(i.Vulkanberg[2].Ware, 0)
	assert.Equal(i.Vulkanberg[2].Posx, 0)
	assert.Equal(i.Vulkanberg[2].Posy, 0)
	assert.Equal(i.Vulkanberg[2].Playerflags, 0)
	assert.Equal(i.Vulkanberg[2].Kind, 0)
	assert.Equal(i.Vulkanberg[2].Empty, 0)
	assert.Equal(i.Vulkanberg[2].Stock, 0)
	assert.Equal(i.Vulkanberg[3].Ware, 0)
	assert.Equal(i.Vulkanberg[3].Posx, 0)
	assert.Equal(i.Vulkanberg[3].Posy, 0)
	assert.Equal(i.Vulkanberg[3].Playerflags, 0)
	assert.Equal(i.Vulkanberg[3].Kind, 0)
	assert.Equal(i.Vulkanberg[3].Empty, 0)
	assert.Equal(i.Vulkanberg[3].Stock, 0)
	assert.Equal(i.Fertility, TobaccoOnly)
	assert.Equal(i.FileNumber, 5)
	assert.Equal(i.Size, LittleIsland)
	assert.Equal(i.Climate, NorthClimate)
	assert.Equal(i.ModifiedFlag, ModifiedFalse)
	assert.Equal(i.Duerrproz, 0)
	assert.Equal(i.Rotier, 0)
	assert.Equal(i.Seeplayerflags, 0)
	assert.Equal(i.Duerrcnt, 0)
}

func TestIsland5FromIsland4(t *testing.T) {
	assert := assert.New(t)
	data := []byte{
		0x49, 0x4E, 0x53, 0x45, 0x4C, 0x34, 0x00, 0xFF, 0x00, 0x00, 0x36, 0x05, 0x00, 0x10, 0x00, 0x00,
		0x74, 0x00, 0x00, 0x00, 0x03, 0x1E, 0x1E, 0x00, 0xAA, 0x00, 0x32, 0x00, 0x04, 0x00, 0x04, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	}
	chunk, err := NewChunk(data)
	assert.Nil(err)
	i4, err := NewIsland4(chunk)
	assert.Nil(err)
	i5 := i4.ToIsland5()
	assert.Equal(i5.Height, 30)
	assert.Equal(i5.Width, 30)
	assert.Equal(i5.IslandNumber, 0)
	assert.Equal(i5.Strtduerrflg, 0)
	assert.Equal(i5.Nofixflg, 0)
	assert.Equal(i5.Vulkanflg, 0)
	assert.Equal(i5.Reserved0, 0)
	assert.Equal(i5.Posx, 170)
	assert.Equal(i5.Posy, 50)
	assert.Equal(i5.Hirschreviercnt, 4)
	assert.Equal(i5.Speedcnt, 4)
	assert.Equal(i5.Stadtplayernr, [11]uint8{0, 0, 0, 0, 0, 0, 0, 0, 0, 0})
	assert.Equal(i5.Vulkancnt, 0)
	assert.Equal(i5.Schatzflg, 0)
	assert.Equal(i5.Rohstanz, 0)
	assert.Equal(i5.Eisencnt, 0)
	assert.Equal(i5.Playerflags, 0)
	assert.Equal(i5.Fertility, Random)
	assert.Equal(i5.Size, LittleIsland)
	assert.Equal(i5.Climate, NorthClimate)
	assert.Equal(i5.ModifiedFlag, ModifiedTrue)
	assert.Equal(i5.Duerrproz, 0)
	assert.Equal(i5.Rotier, 0)
	assert.Equal(i5.Seeplayerflags, 0)
	assert.Equal(i5.Duerrcnt, 0)
	assert.Equal(i5.Eisenberg[0].Ware, 0)
	assert.Equal(i5.Eisenberg[0].Posx, 0)
	assert.Equal(i5.Eisenberg[0].Posy, 0)
	assert.Equal(i5.Eisenberg[0].Playerflags, 0)
	assert.Equal(i5.Eisenberg[0].Kind, 0)
	assert.Equal(i5.Eisenberg[0].Empty, 0)
	assert.Equal(i5.Eisenberg[0].Stock, 0)
	assert.Equal(i5.Eisenberg[1].Ware, 0)
	assert.Equal(i5.Eisenberg[1].Posx, 0)
	assert.Equal(i5.Eisenberg[1].Posy, 0)
	assert.Equal(i5.Eisenberg[1].Playerflags, 0)
	assert.Equal(i5.Eisenberg[1].Kind, 0)
	assert.Equal(i5.Eisenberg[1].Empty, 0)
	assert.Equal(i5.Eisenberg[1].Stock, 0)
	assert.Equal(i5.Eisenberg[2].Ware, 0)
	assert.Equal(i5.Eisenberg[2].Posx, 0)
	assert.Equal(i5.Eisenberg[2].Posy, 0)
	assert.Equal(i5.Eisenberg[2].Playerflags, 0)
	assert.Equal(i5.Eisenberg[2].Kind, 0)
	assert.Equal(i5.Eisenberg[2].Empty, 0)
	assert.Equal(i5.Eisenberg[2].Stock, 0)
	assert.Equal(i5.Eisenberg[3].Ware, 0)
	assert.Equal(i5.Eisenberg[3].Posx, 0)
	assert.Equal(i5.Eisenberg[3].Posy, 0)
	assert.Equal(i5.Eisenberg[3].Playerflags, 0)
	assert.Equal(i5.Eisenberg[3].Kind, 0)
	assert.Equal(i5.Eisenberg[3].Empty, 0)
	assert.Equal(i5.Eisenberg[3].Stock, 0)
	assert.Equal(i5.Vulkanberg[0].Ware, 0)
	assert.Equal(i5.Vulkanberg[0].Posx, 0)
	assert.Equal(i5.Vulkanberg[0].Posy, 0)
	assert.Equal(i5.Vulkanberg[0].Playerflags, 0)
	assert.Equal(i5.Vulkanberg[0].Kind, 0)
	assert.Equal(i5.Vulkanberg[0].Empty, 0)
	assert.Equal(i5.Vulkanberg[0].Stock, 0)
	assert.Equal(i5.Vulkanberg[1].Ware, 0)
	assert.Equal(i5.Vulkanberg[1].Posx, 0)
	assert.Equal(i5.Vulkanberg[1].Posy, 0)
	assert.Equal(i5.Vulkanberg[1].Playerflags, 0)
	assert.Equal(i5.Vulkanberg[1].Kind, 0)
	assert.Equal(i5.Vulkanberg[1].Empty, 0)
	assert.Equal(i5.Vulkanberg[1].Stock, 0)
	assert.Equal(i5.Vulkanberg[2].Ware, 0)
	assert.Equal(i5.Vulkanberg[2].Posx, 0)
	assert.Equal(i5.Vulkanberg[2].Posy, 0)
	assert.Equal(i5.Vulkanberg[2].Playerflags, 0)
	assert.Equal(i5.Vulkanberg[2].Kind, 0)
	assert.Equal(i5.Vulkanberg[2].Empty, 0)
	assert.Equal(i5.Vulkanberg[2].Stock, 0)
	assert.Equal(i5.Vulkanberg[3].Ware, 0)
	assert.Equal(i5.Vulkanberg[3].Posx, 0)
	assert.Equal(i5.Vulkanberg[3].Posy, 0)
	assert.Equal(i5.Vulkanberg[3].Playerflags, 0)
	assert.Equal(i5.Vulkanberg[3].Kind, 0)
	assert.Equal(i5.Vulkanberg[3].Empty, 0)
	assert.Equal(i5.Vulkanberg[3].Stock, 0)
}

func TestIsland5FromIsland3(t *testing.T) {
	assert := assert.New(t)
	data := []byte{
		0x49, 0x4E, 0x53, 0x45, 0x4C, 0x33, 0x00, 0x01, 0x64, 0xFA, 0x12, 0x00, 0xFB, 0x1E, 0x21, 0x10,
		0x28, 0x00, 0x00, 0x00, 0x04, 0x46, 0x3C, 0x00, 0x56, 0x00, 0xDA, 0x00, 0x03, 0x00, 0x04, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	}
	chunk, err := NewChunk(data)
	assert.Nil(err)
	i3, err := NewIsland3(chunk)
	assert.Nil(err)
	i5 := i3.ToIsland5()
	assert.Equal(i5.Height, 60)
	assert.Equal(i5.Width, 70)
	assert.Equal(i5.Posx, 86)
	assert.Equal(i5.Posy, 218)
	assert.Equal(i5.IslandNumber, 4)
}

func TestIslandFileName(t *testing.T) {
	assert := assert.New(t)
	data := []byte{
		0x49, 0x4E, 0x53, 0x45, 0x4C, 0x35, 0x00, 0x7D, 0x40, 0x3E, 0x49, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x74, 0x00, 0x00, 0x00, 0x00, 0x1E, 0x1E, 0x00, 0x18, 0x01, 0x5A, 0x00, 0x01, 0x00, 0x03, 0x00,
		0x06, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00,
		0x02, 0x07, 0x09, 0x00, 0x01, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x83, 0x11, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	}
	chunk, err := NewChunk(data)
	assert.Nil(err)
	assert.Equal(chunk.Id, "INSEL5")
	assert.Equal(chunk.Length, 116)
	i, err := NewIsland5(chunk)
	assert.Nil(err)
	f := i.IslandFileName()
	assert.Equal(f, "nord/lit05.scp")
}
