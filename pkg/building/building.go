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
