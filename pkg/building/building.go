package building

import "image"

type BuildingSizeIdentifier int

const (
	BuildingSize1x1 BuildingSizeIdentifier = iota
	BuildingSize1x2
	BuildingSize1x3
	BuildingSize2x2
	BuildingSize2x1
	BuildingSize2x3
	BuildingSize3x3
	BuildingSize4x4
	BuildingSize4x3
	BuildingSize5x5
	BuildingSize6x6
	BuildingSize6x4
	BuildingSize5x7
	BuildingSizeUnknown
)

func BuildingSize(w, h int) BuildingSizeIdentifier {
	switch {
	case w == 1 && h == 1:
		return BuildingSize1x1
	case w == 1 && h == 2:
		return BuildingSize1x2
	case w == 1 && h == 3:
		return BuildingSize1x3
	case w == 2 && h == 1:
		return BuildingSize2x1
	case w == 2 && h == 2:
		return BuildingSize2x2
	case w == 2 && h == 3:
		return BuildingSize2x3
	case w == 3 && h == 3:
		return BuildingSize3x3
	case w == 4 && h == 3:
		return BuildingSize4x3
	case w == 4 && h == 4:
		return BuildingSize4x4
	case w == 5 && h == 5:
		return BuildingSize5x5
	case w == 6 && h == 6:
		return BuildingSize6x6
	case w == 6 && h == 4:
		return BuildingSize6x4
	case w == 5 && h == 7:
		return BuildingSize5x7
	}

	return BuildingSizeUnknown
}

// Building enthält die Informationen eines Gebäudes
type Building struct {
	Sprite               image.Image
	Id                   int
	BaseIndexSaved       int
	BaseIndex            int
	Rotation             int
	X, Y                 int
	AnimationSteps       int
	CurrentAnimationStep int
	AnimationAdd         int
	Size                 BuildingSizeIdentifier
}

func (b *BuildingSizeIdentifier) Width() int {
	switch *b {
	case BuildingSize1x1, BuildingSize1x2, BuildingSize1x3:
		return 1
	case BuildingSize2x1, BuildingSize2x2, BuildingSize2x3:
		return 2
	case BuildingSize3x3:
		return 3
	case BuildingSize4x3, BuildingSize4x4:
		return 4
	case BuildingSize5x5, BuildingSize5x7:
		return 5
	case BuildingSize6x6, BuildingSize6x4:
		return 6
	default:
		return 0
	}
}

func (b *BuildingSizeIdentifier) Height() int {
	switch *b {
	case BuildingSize1x1, BuildingSize2x1, BuildingSize3x3:
		return 1
	case BuildingSize1x2, BuildingSize2x2, BuildingSize6x4:
		return 2
	case BuildingSize1x3, BuildingSize2x3:
		return 3
	case BuildingSize4x3, BuildingSize4x4:
		return 4
	case BuildingSize5x5, BuildingSize5x7:
		return 5
	case BuildingSize6x6:
		return 6
	default:
		return 0
	}
}

var orderedSizes = []BuildingSizeIdentifier{
	BuildingSize1x2,
	BuildingSize2x1,
	BuildingSize2x2,
	BuildingSize2x3,
	BuildingSize4x3,
	BuildingSize5x5,
	BuildingSize6x4,
	BuildingSize6x6,
	BuildingSize1x3,
	BuildingSize3x3,
	BuildingSize4x4,
	BuildingSize1x1,
	BuildingSize5x7,
}

func NextBuildingSize(current BuildingSizeIdentifier) BuildingSizeIdentifier {
	for i, size := range orderedSizes {
		if size == current && i+1 < len(orderedSizes) {
			return orderedSizes[i+1]
		}
	}
	return orderedSizes[0]
}

func PreviousBuildingSize(current BuildingSizeIdentifier) BuildingSizeIdentifier {
	for i, size := range orderedSizes {
		if size == current && i > 0 {
			return orderedSizes[i-1]
		}
	}
	return orderedSizes[len(orderedSizes)-1]
}

var SizeToIdentifier = map[BuildingSizeIdentifier]string{
	BuildingSize1x1:     "1x1",
	BuildingSize1x2:     "1x2",
	BuildingSize1x3:     "1x3",
	BuildingSize2x1:     "2x1",
	BuildingSize2x2:     "2x2",
	BuildingSize2x3:     "2x3",
	BuildingSize3x3:     "3x3",
	BuildingSize4x3:     "4x3",
	BuildingSize4x4:     "4x4",
	BuildingSize5x5:     "5x5",
	BuildingSize6x6:     "6x6",
	BuildingSize6x4:     "6x4",
	BuildingSize5x7:     "5x7",
	BuildingSizeUnknown: "unknown",
}

func (size BuildingSizeIdentifier) String() string {
	if id, ok := SizeToIdentifier[size]; ok {
		return id
	}
	return SizeToIdentifier[BuildingSizeUnknown]
}
