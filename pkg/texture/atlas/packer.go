package atlas

import (
	"fmt"
	"math"

	uuid "github.com/google/uuid"
)

func (p *MaxRectsPacker) findFreeRect(rectWidth, rectHeight int) (Rect, int, error) {
	bestRectIndex := -1
	bestScore := math.MaxInt
	for i, freeRect := range p.FreeRects {
		if freeRect.Width >= rectWidth && freeRect.Height >= rectHeight {
			score := p.scoreRect(freeRect, rectWidth, rectHeight)
			if score < bestScore {
				bestScore = score
				bestRectIndex = i
			}
		}
	}

	if bestRectIndex == -1 {
		return Rect{}, 0, fmt.Errorf("unable to pack rectangle of size %dx%d", rectWidth, rectHeight)
	}
	return p.FreeRects[bestRectIndex], bestRectIndex, nil
}

// Pack tries to pack the given rectangle into the texture atlas
func (p *MaxRectsPacker) Pack(rectWidth, rectHeight int) (Rect, error) {
	// Find the smallest free rectangle that can fit the image
	bestRect, index, err := p.findFreeRect(rectWidth, rectHeight)
	if err != nil {
		return Rect{}, err
	}

	// Split the free rectangle into two new rectangles (remaining free space)
	p.splitFreeRect(index, rectWidth, rectHeight)

	// // Add the packed rectangle to the occupied list
	// p.OccupiedRects = append(p.OccupiedRects, Rect{
	// 	X:      bestRect.X,
	// 	Y:      bestRect.Y,
	// 	Width:  rectWidth,
	// 	Height: rectHeight,
	// })

	return Rect{X: bestRect.X, Y: bestRect.Y, Width: rectWidth, Height: rectHeight}, nil
}

// scoreRect calculates a score for the given placement of a rectangle
func (p *MaxRectsPacker) scoreRect(freeRect Rect, rectWidth, rectHeight int) int {
	return int(math.Max(float64(freeRect.Width-rectWidth), float64(freeRect.Height-rectHeight)))
}

// splitFreeRect splits a free rectangle into two new rectangles
func (p *MaxRectsPacker) splitFreeRect(index, rectWidth, rectHeight int) {
	freeRect := p.FreeRects[index]

	// Remove the original free rectangle
	p.FreeRects = append(p.FreeRects[:index], p.FreeRects[index+1:]...)

	// Split the free rectangle horizontally if there's enough space
	if freeRect.Width > rectWidth {
		newRect := Rect{
			X:      freeRect.X + rectWidth,
			Y:      freeRect.Y,
			Width:  freeRect.Width - rectWidth,
			Height: rectHeight,
			UUID:   uuid.New().String(),
		}
		if newRect.X == 0 && newRect.Y == 1313 && newRect.Width == 1024 && newRect.Height == 68 {
			fmt.Println("newRect:", newRect)
		}
		if newRect.Width >= 64 && newRect.Height >= 31 {
			p.FreeRects = append(p.FreeRects, newRect)
		}
	}

	// Split the free rectangle vertically if there's enough space
	if freeRect.Height > rectHeight {
		newRect := Rect{
			X:      freeRect.X,
			Y:      freeRect.Y + rectHeight,
			Width:  freeRect.Width,
			Height: freeRect.Height - rectHeight,
			UUID:   uuid.New().String(),
		}
		if newRect.X == 0 && newRect.Y == 1313 && newRect.Width == 1024 && newRect.Height == 68 {
			fmt.Println("newRect:", newRect)
		}
		if newRect.Width >= 64 && newRect.Height >= 31 {
			p.FreeRects = append(p.FreeRects, newRect)
		}
	}
}

// Rect represents a rectangular region
type Rect struct {
	X, Y, Width, Height int
	UUID                string
}

// MaxRectsPacker represents a texture atlas packer using the maximal rectangles algorithm
type MaxRectsPacker struct {
	Width, Height int
	FreeRects     []Rect
	// OccupiedRects []Rect
}

// NewMaxRectsPacker creates a new MaxRectsPacker
func NewMaxRectsPacker(width, height int) *MaxRectsPacker {
	return &MaxRectsPacker{
		Width:  width,
		Height: height,
		FreeRects: []Rect{
			Rect{0, 0, width, height, uuid.New().String()},
		},
	}
}
