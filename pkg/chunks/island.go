package chunks

type OreMountainData struct {
	Ware        int `bitfield:"8"`  // which good lays here?
	Posx        int `bitfield:"8"`  // position on island x
	Posy        int `bitfield:"8"`  // position on island y
	Playerflags int `bitfield:"8"`  // which player knows this secret? (DANGER: PLAYER_MAX)
	Kind        int `bitfield:"8"`  // which kind?
	Empty       int `bitfield:"8"`  // empty
	Stock       int `bitfield:"16"` // how much of this good lays here?
}

type Fertility uint32

const (
	Random           Fertility = 0x0000
	None             Fertility = 0x1181
	TobaccoOnly      Fertility = 0x1183
	WineOnly         Fertility = 0x11A1
	SugarOnly        Fertility = 0x1189
	CocoaOnly        Fertility = 0x11C1
	WoolOnly         Fertility = 0x1191
	SpicesOnly       Fertility = 0x1185
	TobaccoAndSpices Fertility = 0x1187
	TobaccoAndSugar  Fertility = 0x118B
	SpicesAndSugar   Fertility = 0x118D
	WoolAndWine      Fertility = 0x11B1
	WoolAndCocoa     Fertility = 0x11D1
	WineAndCocoa     Fertility = 0x11E1
)

type IslandModified uint8

const (
	ModifiedFalse IslandModified = 0
	ModifiedTrue  IslandModified = 1
)

type IslandSize uint16

const (
	LittleIsland IslandSize = 0
	MiddleIsland IslandSize = 1
	MedianIsland IslandSize = 2
	BigIsland    IslandSize = 3
	LargeIsland  IslandSize = 4
)

type IslandClimate uint8

const (
	NorthClimate IslandClimate = 0
	SouthClimate IslandClimate = 1
	AnyClimate   IslandClimate = 2
)

var IslandSizeMap = map[IslandSize]string{
	LittleIsland: "lit",
	MiddleIsland: "mit",
	MedianIsland: "med",
	BigIsland:    "big",
	LargeIsland:  "lar",
}

var IslandClimateMap = map[IslandClimate]string{
	SouthClimate: "sued",
	NorthClimate: "nord",
	AnyClimate:   "any",
}

type TileGraphic struct {
	Index        int16
	GroundHeight uint8
}
