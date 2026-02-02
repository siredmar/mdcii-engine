package chunks

import (
	"fmt"
	"log"

	"github.com/siredmar/mdcii-engine/pkg/cod/buildings"
)

const (
	IslandHouseFieldSize = 8
)

type Field struct {
	Id   int `json:"id"`    // tile gaphic ID, see haeuser.cod for referene
	Posx int `json:"pos_x"` // position on island
	Posy int `json:"pos_y"` // position on island
	// X              int            `json:"x"`         // X position within the building
	// Y              int            `json:"y"`         // Y position within the building
	Orientation    int            `json:"rotation"`  // orientation
	AnimationCount int            `json:"animation"` // animation step for tile
	IslandNumber   int            `json:"island"`    // the island the field is part of
	CityNumber     int            `json:"city"`      // the city the field is part of
	RandomNumber   int            `json:"random"`    // random number, what for?
	PlayerNumber   int            `json:"player"`    // the player that occupies this field
	Reserved       int            `json:"reserved"`  // is this field empty? always 51?
	Kind           buildings.Kind `json:"kind"`      // kind of field, always 0?
}

type IslandHouse struct {
	Size        IslandDimensions     `json:"size"`
	Fields      []Field              `json:"fields"`
	RawElements int                  `json:"-"`
	RawFields   []Field              `json:"-"`
	Buildings   *buildings.Buildings `json:"-"`
}

type IslandDimensions struct {
	Width  int
	Height int
}

func NewIslandHouse(c *Chunk, size IslandDimensions, b *buildings.Buildings) (*IslandHouse, error) {
	islandhouse := &IslandHouse{
		Size:        size,
		RawElements: c.Length / IslandHouseFieldSize,
		Fields:      make([]Field, 0),
		RawFields:   make([]Field, 0),
		Buildings:   b,
	}

	for i := 0; i < c.Length; i = i + IslandHouseFieldSize {
		fieldData := c.Data[i : i+IslandHouseFieldSize]

		bits := uint32(fieldData[4]) | uint32(fieldData[5])<<8 | uint32(fieldData[6])<<16 | uint32(fieldData[7])<<24
		id := int(fieldData[0]) | int(fieldData[1])<<8
		if id == 102 {
			id = 169
		}
		index := 0
		if b != nil {
			var err error
			index, err = b.GetBuildingIndexById(id)
			if err != nil {
				log.Printf("Skipping unknown building ID %d at pos (%d,%d)", id, int(fieldData[2]), int(fieldData[3]))
				continue
			}
		}
		field := &Field{
			Id:             id,
			Posx:           int(fieldData[2]),
			Posy:           int(fieldData[3]),
			Orientation:    int((bits >> 0) & ((1 << 2) - 1)),
			AnimationCount: int((bits >> 2) & ((1 << 4) - 1)),
			IslandNumber:   int((bits >> 6) & ((1 << 8) - 1)),
			CityNumber:     int((bits >> 14) & ((1 << 3) - 1)),
			RandomNumber:   int((bits >> 17) & ((1 << 5) - 1)),
			PlayerNumber:   int((bits >> 22) & ((1 << 4) - 1)),
			Reserved:       int((bits >> 26) & ((1 << 6) - 1)),
			// X:              0,
			// Y:              0,
		}
		if b != nil {
			field.Kind = b.BuildingsVector[index].Kind
		}
		islandhouse.RawFields = append(islandhouse.RawFields, *field)
	}
	islandhouse.finalize()
	return islandhouse, nil
}

func NewEmptyIslandHouse(size IslandDimensions) *IslandHouse {
	i := &IslandHouse{
		Fields: make([]Field, size.Width*size.Height),
		Size:   size,
	}
	i.finalize()
	return i
}

func (i *IslandHouse) finalize() {
	i.Fields = make([]Field, i.Size.Height*i.Size.Width)
	for y := 0; y < i.Size.Height; y++ {
		for x := 0; x < i.Size.Width; x++ {
			i.Fields[y*i.Size.Width+x] = Field{Id: 0xFFFF, Posx: x, Posy: y}
		}
	}
	for _, tile := range i.RawFields {
		if tile.Id == 102 {
			fmt.Printf("ID: %d, X: %d, Y: %d\n", tile.Id, tile.Posx, tile.Posy)
			tile.Id = 169
		}
		if tile.Posx >= i.Size.Width || tile.Posy >= i.Size.Height {
			continue
		}
		if i.Buildings != nil {
			_, err := i.Buildings.GetBuilding(tile.Id)
			if err != nil {
				log.Println(err)
				continue
			}
		}
		idx := tile.Posy*i.Size.Width + tile.Posx
		if idx >= 0 && idx < len(i.Fields) {
			i.Fields[idx] = tile
		}
	}
}

func (i *IslandHouse) Get(x, y int) Field {
	return i.Fields[y*i.Size.Width+x]
}

func (i *IslandHouse) GetSize() int {
	return len(i.Fields)
}
