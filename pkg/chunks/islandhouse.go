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
	Id             int `json:"id"`        // tile gaphic ID, see haeuser.cod for referene
	Posx           int `json:"pos_x"`     // position on island
	Posy           int `json:"pos_y"`     // position on island
	X              int `json:"x"`         // X position within the building
	Y              int `json:"y"`         // Y position within the building
	Orientation    int `json:"rotation"`  // orientation
	AnimationCount int `json:"animation"` // animation step for tile
	IslandNumber   int `json:"island"`    // the island the field is part of
	CityNumber     int `json:"city"`      // the city the field is part of
	RandomNumber   int `json:"random"`    // random number, what for?
	PlayerNumber   int `json:"player"`    // the player that occupies this field
	Reserved       int `json:"reserved"`  // is this field empty? always 51?
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

		field := &Field{
			Id:             int(fieldData[0]) | int(fieldData[1])<<8,
			Posx:           int(fieldData[2]),
			Posy:           int(fieldData[3]),
			Orientation:    int((bits >> 0) & ((1 << 2) - 1)),
			AnimationCount: int((bits >> 2) & ((1 << 4) - 1)),
			IslandNumber:   int((bits >> 6) & ((1 << 8) - 1)),
			CityNumber:     int((bits >> 14) & ((1 << 3) - 1)),
			RandomNumber:   int((bits >> 17) & ((1 << 5) - 1)),
			PlayerNumber:   int((bits >> 22) & ((1 << 4) - 1)),
			Reserved:       int((bits >> 26) & ((1 << 6) - 1)),
			X:              0,
			Y:              0,
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
			// Setting default ID meaning 'tile not set'. This gets overwritten later on if on this x,y, position is a valid tile
			i.Fields[y*i.Size.Width+x].Id = 0xFFFF
		}
	}
	// Now iterate through the passed 'data'. This is the read chunk containing one layer. The layer might contain the bare island
	// or some houses. So it's checked if on the position is a valid field. This step is done to make it easier to calculate
	// graphic indexes for elements bigger than 1,1. The 'posx' and 'posy' fields are used to store the fields partly position if bigger
	// than 1,1 because the position is also given via the array index. So no information is being lost if overwriting 'posx' and 'posy'.
	for _, tile := range i.RawFields {
		if tile.Id == 102 {
			fmt.Printf("ID: %d, X: %d, Y: %d\n", tile.Id, tile.Posx, tile.Posy)
			tile.Id = 169
		}
		if tile.Posx >= i.Size.Width || tile.Posy >= i.Size.Height {
			continue
		}
		// elementWidth := 0
		// elementHeight := 0
		if i.Buildings != nil {
			_, err := i.Buildings.GetBuilding(tile.Id)
			if err != nil {
				log.Println(err)
				continue
				// i.Fields[tile.Posy*i.Size.Width+tile.Posx].X = 0
				// i.Fields[tile.Posy*i.Size.Width+tile.Posx].Y = 0
				// continue
				// } else {
				// if tile.Orientation%2 == 0 {
				// 	elementHeight = info.Size.H
				// 	elementWidth = info.Size.W
				// } else {
				// 	elementHeight = info.Size.W
				// 	elementWidth = info.Size.H
				// }
				// elementHeight = 1
				// elementWidth = 1
				// }
			}
			i.Fields[tile.Posy*i.Size.Width+tile.Posx] = tile
			// if elementWidth > 1 || elementHeight > 1 {
			// 	fmt.Println("Element bigger than 1,1")
			// }
			// for y := 0; y < elementHeight && tile.Posy+y < i.Size.Height; y++ {
			// 	for x := 0; x < elementWidth && tile.Posx+x < i.Size.Width; x++ {
			// 		index := (tile.Posy+y)*i.Size.Width + (tile.Posx + x)
			// 		i.Fields[index] = tile
			// 		i.Fields[index].X = x
			// 		i.Fields[index].Y = y
			// 		// fmt.Println(i.Fields[index])
			// 	}
			// }
		}
	}
}

func (i *IslandHouse) Get(x, y int) Field {
	return i.Fields[y*i.Size.Width+x]
}

func (i *IslandHouse) GetSize() int {
	return len(i.Fields)
}
