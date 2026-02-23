package atlas

import (
	"fmt"
	"math"
)

func (p *MaxRectsPacker) findFreeRect(rectWidth, rectHeight int) (Rect, int, bool, error) {
	bestRectIndex := -1
	bestScore := math.MaxInt
	bestRotated := false
	for i, freeRect := range p.FreeRects {
		// Try without rotation
		if freeRect.Width >= rectWidth && freeRect.Height >= rectHeight {
			score := p.scoreRect(freeRect, rectWidth, rectHeight)
			if score < bestScore {
				bestScore = score
				bestRectIndex = i
				bestRotated = false
			}
		}
		// Try with rotation
		if freeRect.Width >= rectHeight && freeRect.Height >= rectWidth {
			score := p.scoreRect(freeRect, rectHeight, rectWidth)
			if score < bestScore {
				bestScore = score
				bestRectIndex = i
				bestRotated = true
			}
		}
	}

	if bestRectIndex == -1 {
		return Rect{}, 0, false, fmt.Errorf("unable to pack rectangle of size %dx%d", rectWidth, rectHeight)
	}
	return p.FreeRects[bestRectIndex], bestRectIndex, bestRotated, nil
}

// Pack tries to pack the given rectangle into the texture atlas
func (p *MaxRectsPacker) Pack(rectWidth, rectHeight int) (Rect, bool, error) {
	bestRect, index, rotated, err := p.findFreeRect(rectWidth, rectHeight)
	if err != nil {
		return Rect{}, false, err
	}
	w, h := rectWidth, rectHeight
	if rotated {
		w, h = rectHeight, rectWidth
	}
	p.splitFreeRect(index, w, h)
	return Rect{X: bestRect.X, Y: bestRect.Y, Width: w, Height: h}, rotated, nil
}

// scoreRect calculates a score for the given placement of a rectangle (Best Area Fit)
func (p *MaxRectsPacker) scoreRect(freeRect Rect, rectWidth, rectHeight int) int {
	areaFit := (freeRect.Width * freeRect.Height) - (rectWidth * rectHeight)
	shortSideFit := min(abs(freeRect.Width-rectWidth), abs(freeRect.Height-rectHeight))
	return areaFit*1000 + shortSideFit // prioritize area fit, then short side fit
}

func abs(x int) int {
	if x < 0 {
		return -x
	}
	return x
}

func min(a, b int) int {
	if a < b {
		return a
	}
	return b
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
		}
		p.FreeRects = append(p.FreeRects, newRect)
	}

	// Split the free rectangle vertically if there's enough space
	if freeRect.Height > rectHeight {
		newRect := Rect{
			X:      freeRect.X,
			Y:      freeRect.Y + rectHeight,
			Width:  freeRect.Width,
			Height: freeRect.Height - rectHeight,
		}
		p.FreeRects = append(p.FreeRects, newRect)
	}
}

// Rect represents a rectangular region
type Rect struct {
	X, Y, Width, Height int
}

// MaxRectsPacker represents a texture atlas packer using the maximal rectangles algorithm
type MaxRectsPacker struct {
	Width, Height int
	FreeRects     []Rect
}

// NewMaxRectsPacker creates a new MaxRectsPacker
func NewMaxRectsPacker(width, height int) *MaxRectsPacker {
	return &MaxRectsPacker{
		Width:     width,
		Height:    height,
		FreeRects: []Rect{{0, 0, width, height}},
	}
}
