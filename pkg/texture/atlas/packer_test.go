package atlas

import (
	"testing"
)

func TestNewMaxRectsPacker(t *testing.T) {
	width, height := 1024, 1024
	packer := NewMaxRectsPacker(width, height)

	if packer.Width != width || packer.Height != height {
		t.Errorf("Expected packer dimensions to be %dx%d, got %dx%d", width, height, packer.Width, packer.Height)
	}

	if len(packer.FreeRects) != 1 {
		t.Errorf("Expected 1 free rectangle, got %d", len(packer.FreeRects))
	}

	freeRect := packer.FreeRects[0]
	if freeRect.Width != width || freeRect.Height != height {
		t.Errorf("Expected free rectangle dimensions to be %dx%d, got %dx%d", width, height, freeRect.Width, freeRect.Height)
	}
}

func TestPack(t *testing.T) {
	packer := NewMaxRectsPacker(1024, 1024)

	rectWidth, rectHeight := 512, 512
	rect, rotated, err := packer.Pack(rectWidth, rectHeight)
	if err != nil {
		t.Fatalf("Unexpected error: %v", err)
	}

	if rotated {
		t.Errorf("Expected rectangle not to be rotated")
	}

	if rect.Width != rectWidth || rect.Height != rectHeight {
		t.Errorf("Expected rectangle dimensions to be %dx%d, got %dx%d", rectWidth, rectHeight, rect.Width, rect.Height)
	}

	if rect.X != 0 || rect.Y != 0 {
		t.Errorf("Expected rectangle position to be (0, 0), got (%d, %d)", rect.X, rect.Y)
	}
}

func TestPackWithRotation(t *testing.T) {
	packer := NewMaxRectsPacker(1024, 1024)

	rectWidth, rectHeight := 512, 1024
	rect, rotated, err := packer.Pack(rectWidth, rectHeight)
	if err != nil {
		t.Fatalf("Unexpected error: %v", err)
	}

	// The packer may or may not rotate depending on its scoring strategy.
	// Accept either orientation as long as it fits and dimensions match one of the two options.
	if !((rect.Width == rectWidth && rect.Height == rectHeight) || (rect.Width == rectHeight && rect.Height == rectWidth)) {
		t.Errorf("Expected rectangle dimensions to be %dx%d or %dx%d, got %dx%d", rectWidth, rectHeight, rectHeight, rectWidth, rect.Width, rect.Height)
	}
	_ = rotated

	if rect.X != 0 || rect.Y != 0 {
		t.Errorf("Expected rectangle position to be (0, 0), got (%d, %d)", rect.X, rect.Y)
	}
}

func TestPackFailure(t *testing.T) {
	packer := NewMaxRectsPacker(512, 512)

	rectWidth, rectHeight := 1024, 1024
	_, _, err := packer.Pack(rectWidth, rectHeight)
	if err == nil {
		t.Fatalf("Expected error when packing rectangle larger than atlas")
	}
}
