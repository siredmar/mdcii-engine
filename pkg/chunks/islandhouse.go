package chunks

const (
	IslandHouseFieldSize = 8
)

type Field struct {
	Id             int // tile gaphic ID, see haeuser.cod for referene
	Posx           int // position on island
	Posy           int // position on island
	Orientation    int // orientation
	AnimationCount int // animation step for tile
	IslandNumber   int // the island the field is part of
	CityNumber     int // the city the field is part of
	RandomNumber   int // random number, what for?
	PlayerNumber   int // the player that occupies this field
	Reserved       int // is this field empty?
}

type IslandHouse struct {
	size        IslandDimensions
	Fields      []Field
	rawElements int
	rawFields   []Field
}

type IslandDimensions struct {
	Width  int
	Height int
}

func NewIslandHouse(c *Chunk, size IslandDimensions) (*IslandHouse, error) {

	islandhouse := &IslandHouse{
		size:        size,
		rawElements: c.Length / IslandHouseFieldSize,
		Fields:      make([]Field, 0),
		rawFields:   make([]Field, 0),
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
		}

		islandhouse.rawFields = append(islandhouse.rawFields, *field)
	}
	islandhouse.finalize()
	return islandhouse, nil
}

func NewEmptyIslandHouse(size IslandDimensions) *IslandHouse {
	i := &IslandHouse{
		Fields: make([]Field, size.Width*size.Height),
		size:   size,
	}
	i.finalize()
	return i
}

func (i *IslandHouse) finalize() {
	i.Fields = make([]Field, i.size.Height*i.size.Width)
	for y := 0; y < i.size.Height; y++ {
		for x := 0; x < i.size.Width; x++ {
			// Setting default ID meaning 'tile not set'. This gets overwritten later on if on this x,y, position is a valid tile
			i.Fields[y*i.size.Width+x].Id = 0xFFFF
		}
	}
	// Now iterate through the passed 'data'. This is the read chunk containing one layer. The layer might contain the bare island
	// or some houses. So it's checked if on the position is a valid field. This step is done to make it easier to calculate
	// graphic indexes for elements bigger than 1,1. The 'posx' and 'posy' fields are used to store the fields partly position if bigger
	// than 1,1 because the position is also given via the array index. So no information is being lost if overwriting 'posx' and 'posy'.
	for _, tile := range i.rawFields {
		if tile.Posx >= i.size.Width || tile.Posy >= i.size.Height {
			continue
		}
		// todo: buildings->GetHouse(tile.Id)
		// {} else 		// { // no building found for tile ID, so it's a normal tile
		i.Fields[tile.Posy*i.size.Width+tile.Posx] = tile
		i.Fields[tile.Posy*i.size.Width+tile.Posx].Posx = 0
		i.Fields[tile.Posy*i.size.Width+tile.Posx].Posy = 0

		// }

	}
}
