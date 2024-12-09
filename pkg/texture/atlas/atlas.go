package atlas

import (
	"encoding/json"
	"fmt"
	"image"
	"image/draw"
	"image/png"
	"log"
	"os"
	"path/filepath"
	"time"

	"github.com/siredmar/mdcii-engine/pkg/bsh"
	"github.com/siredmar/mdcii-engine/pkg/building"
	"github.com/siredmar/mdcii-engine/pkg/world/rotation"

	"github.com/hajimehoshi/ebiten/v2"
	buildingsCOD "github.com/siredmar/mdcii-engine/pkg/cod/buildings"
)

type Animation struct {
	Images []Image
	Steps  int
	Time   time.Duration
}

type ImageSetRotation struct {
	Animations map[rotation.Rotation]*Animation
}

// TextureAtlas represents a texture atlas containing multiple images
type TextureAtlas struct {
	Images    []*image.RGBA `json:"-"`
	AtlasMeta AtlasMeta     `json:"atlasMeta"`
	// map[buildingIndex]map[rotation][]Images - slice is for animations
	ImagesMeta           map[int]*ImageSetRotation `json:"imageMeta"`
	OptionSkipFileEnding bool                      `json:"-"`
	OptionKeyToLower     bool                      `json:"-"`
	OptionKeyToUpper     bool                      `json:"-"`
	PNGs                 *bsh.BshPng
	BuildingsCOD         buildingsCOD.Buildings
	// filesToLoad          []string `json:"-"`
	outputDir string `json:"-"`
	indexToId map[int]int
	idToIndex map[int]int
	// imagesToLoad         map[string]image.Image   `json:"-"`
}

type AtlasMeta struct {
	Width  int    `json:"width"`
	Height int    `json:"height"`
	Name   string `json:"name"`
}

// Metadata contains metadata for an image in the atlas
type Metadata struct {
	BuildingID     int `json:"buildingID"`
	Width          int `json:"width"`
	Height         int `json:"height"`
	X              int `json:"x"`
	Y              int `json:"y"`
	Rotation       int `json:"rotation"`
	AnimationIndex int `json:"animationIndex"`
}

// Image contains metadata for an image in the atlas
type Image struct {
	Sprite   image.Image
	Metadata Metadata `json:"metadata"`
}

type TextureAtlasOption func(*TextureAtlas)

// func WithSkipFileEnding() TextureAtlasOption {
// 	return func(h *TextureAtlas) {
// 		h.OptionSkipFileEnding = true
// 	}
// }

func WithKeyToLower() TextureAtlasOption {
	return func(h *TextureAtlas) {
		h.OptionKeyToLower = true
	}
}

func WithKeyToUpper() TextureAtlasOption {
	return func(h *TextureAtlas) {
		h.OptionKeyToUpper = true
	}
}

func WithName(name string) TextureAtlasOption {
	return func(h *TextureAtlas) {
		h.AtlasMeta.Name = name
	}
}

func WithImages(pngs *bsh.BshPng) TextureAtlasOption {
	return func(h *TextureAtlas) {
		h.PNGs = pngs
	}
}

// func WithFiles(files []string) TextureAtlasOption {
// 	return func(h *TextureAtlas) {
// 		h.filesToLoad = files
// 	}
// }

func WithOutputDir(outputDir string) TextureAtlasOption {
	return func(h *TextureAtlas) {
		h.outputDir = outputDir
	}
}

type TileSize struct {
	Width  int
	Height int
}

// findContentBounds determines the bounding rectangle of non-transparent pixels in an Ebiten image.
func (a *TextureAtlas) findContentBounds(eimg *ebiten.Image) image.Rectangle {
	bounds := eimg.Bounds()
	minX, minY := bounds.Max.X, bounds.Max.Y
	maxX, maxY := bounds.Min.X, bounds.Min.Y

	for y := bounds.Min.Y; y < bounds.Max.Y; y++ {
		for x := bounds.Min.X; x < bounds.Max.X; x++ {
			_, _, _, alpha := eimg.At(x, y).RGBA()
			if alpha > 0 { // Non-transparent pixel
				if x < minX {
					minX = x
				}
				if y < minY {
					minY = y
				}
				if x > maxX {
					maxX = x
				}
				if y > maxY {
					maxY = y
				}
			}
		}
	}

	// Ensure valid bounds
	if minX > maxX || minY > maxY {
		return image.Rect(0, 0, 0, 0) // No content
	}

	return image.Rect(minX, minY, maxX+1, maxY+1) // Add 1 to include the last pixel
}

const (
	tileWidth  = 64
	tileHeight = 31
)

func (a *TextureAtlas) drawBuildingToImage(b *building.Building, tileSize TileSize) image.Image {
	// Create a blank RGBA image for drawing
	outputImage := image.NewRGBA(image.Rect(0, 0, 1000, 1000))

	// Draw the building
	offsets := building.RotationOffsets[b.Size][b.Rotation]
	for i, offset := range offsets {
		screenX := b.X + (offset[0]-offset[1])*(tileSize.Width/2)
		screenY := b.Y + (offset[0]+offset[1])*(tileSize.Height/2)

		// textureKey := fmt.Sprintf("%d", b.BaseIndex+i)
		textureKey := func(baseIndex, rotation, tileIndex int, size building.BuildingSizeIdentifier) string {
			tilesPerRotation := len(building.RotationOffsets[size][rotation])
			return fmt.Sprintf("%d", baseIndex+(rotation*tilesPerRotation)+tileIndex)
		}(b.BaseIndex, b.Rotation, i, b.Size)

		tileImg, ok := a.PNGs.Images[textureKey]
		if !ok {
			log.Printf("Texture key %s not found in texture atlas", textureKey)
			continue
		}

		baseOffsetY := func(tileHeight, gridTileHeight int) int {
			if tileHeight > gridTileHeight {
				return tileHeight - gridTileHeight
			}
			return 0
		}(tileImg.Bounds().Dy(), tileHeight)

		draw.Draw(outputImage, image.Rect(screenX, screenY-baseOffsetY, screenX+tileSize.Width, screenY+tileSize.Height),
			tileImg, image.Point{}, draw.Over)
	}

	// Find the bounds of the non-alpha content
	cropBounds := findNonAlphaBounds(outputImage)

	// Crop the image to the determined bounds
	croppedImage := cropImage(outputImage, cropBounds)
	return croppedImage
	// // Save the cropped output image as a PNG file
	// outputFile, err := os.Create("output.png")
	// if err != nil {
	// 	log.Fatal(err)
	// }
	// defer outputFile.Close()

	// err = png.Encode(outputFile, croppedImage)
	// if err != nil {
	// 	log.Fatal(err)
	// }
	// fmt.Println("Image rendered, cropped, and saved as output.png")
}

// findNonAlphaBounds determines the bounds of the non-transparent content in an image.
func findNonAlphaBounds(img *image.RGBA) image.Rectangle {
	bounds := img.Bounds()
	minX, minY := bounds.Max.X, bounds.Max.Y
	maxX, maxY := bounds.Min.X, bounds.Min.Y

	for y := bounds.Min.Y; y < bounds.Max.Y; y++ {
		for x := bounds.Min.X; x < bounds.Max.X; x++ {
			_, _, _, a := img.At(x, y).RGBA()
			if a > 0 { // Check for non-transparent pixel
				if x < minX {
					minX = x
				}
				if x > maxX {
					maxX = x
				}
				if y < minY {
					minY = y
				}
				if y > maxY {
					maxY = y
				}
			}
		}
	}

	// Ensure valid bounds are returned
	if minX > maxX || minY > maxY {
		return image.Rect(0, 0, 0, 0) // No content found
	}
	return image.Rect(minX, minY, maxX+1, maxY+1)
}

// cropImage crops an image to the specified rectangle.
func cropImage(img *image.RGBA, rect image.Rectangle) *image.RGBA {
	cropped := image.NewRGBA(rect)
	draw.Draw(cropped, rect, img, rect.Min, draw.Src)
	return cropped
}

// New creates a texture atlas from a list of image filenames
func New(atlasWidth, atlasHeight int, buildings *buildingsCOD.Buildings, opts ...TextureAtlasOption) (*TextureAtlas, error) {

	atlas := &TextureAtlas{
		Images:     []*image.RGBA{image.NewRGBA(image.Rect(0, 0, atlasWidth, atlasHeight))},
		ImagesMeta: make(map[int]*ImageSetRotation),
		AtlasMeta: AtlasMeta{
			Width:  atlasWidth,
			Height: atlasHeight,
			Name:   "atlas",
		},
		BuildingsCOD: *buildings,
		// imagesToLoad: make(map[string]image.Image),
		// filesToLoad:  []string{},
		outputDir: ".",
		indexToId: make(map[int]int),
		idToIndex: make(map[int]int),
	}

	// Loop through each option
	for _, opt := range opts {
		opt(atlas)
	}

	for _, buildingCOD := range buildings.BuildingsVector {
		buildingID := buildingCOD.Id
		if buildingID == 2121 {
			fmt.Println("Building ID:", buildingID)
		}
		// atlas.indexToId[i] = buildingID
		// atlas.idToIndex[buildingID] = i
		// rotationsCod := buildingCOD.Rotate
		// rotations := 4
		// if rotationsCod == 0 {
		// 	rotations = 1
		// }

		b := &building.Building{
			Id:                   buildingCOD.Id,
			BaseIndexSaved:       buildingCOD.Gfx,
			BaseIndex:            buildingCOD.Gfx,
			Rotation:             0,
			AnimationSteps:       buildingCOD.AnimationAmount,
			CurrentAnimationStep: 0,
			AnimationAdd:         buildingCOD.AnimationAdd,
			X:                    100,
			Y:                    100,
			Size:                 building.BuildingSize(buildingCOD.Size.W, buildingCOD.Size.H),
		}

		for rot := range []rotation.Rotation{rotation.DEG0, rotation.DEG90, rotation.DEG180, rotation.DEG270} {
			animations := buildingCOD.AnimationAmount
			if buildingCOD.AnimationAmount == 0 {
				animations = 1
			}
			for animationStep := 0; animationStep < animations; animationStep++ {
				img := atlas.drawBuildingToImage(b, TileSize{Width: tileWidth, Height: tileHeight})
				if atlas.ImagesMeta[buildingID] == nil {
					atlas.ImagesMeta[buildingID] = &ImageSetRotation{
						Animations: make(map[rotation.Rotation]*Animation),
					}
				}

				if atlas.ImagesMeta[buildingID].Animations[rotation.Rotation(rot)] == nil {
					atlas.ImagesMeta[buildingID].Animations[rotation.Rotation(rot)] = &Animation{
						Images: []Image{},
						Steps:  animations,
					}
				}

				atlas.ImagesMeta[buildingID].Animations[rotation.Rotation(rot)].Images = append(atlas.ImagesMeta[buildingID].Animations[rotation.Rotation(rot)].Images, Image{
					Sprite: img,
					Metadata: Metadata{
						BuildingID: buildingID,
						Width:      img.Bounds().Dx(),
						Height:     img.Bounds().Dy(),
						// X:              get set during packing,
						// Y:              get set during packing,
						Rotation:       b.Rotation,
						AnimationIndex: animationStep,
					},
				})
				atlas.ImagesMeta[buildingID].Animations[rotation.Rotation(rot)].Time = time.Duration((1000.0 / buildingCOD.AnimationTime) / 60.0)
				if b.AnimationSteps > 0 {
					b.CurrentAnimationStep = (b.CurrentAnimationStep + 1) % b.AnimationSteps
					add := b.CurrentAnimationStep * b.AnimationAdd
					b.BaseIndex = b.BaseIndexSaved + add
				}
			}
			b.Rotation = (b.Rotation + 1) % 4
		}
	}

	// packer := NewMaxRectsPacker(atlasWidth, atlasHeight)
	for id, img := range atlas.ImagesMeta {
		for rot, animations := range img.Animations {
			fmt.Printf("ID: %d, Rot: %d, Animations: %d, BuildingID: %d\n", id, rot, len(animations.Images), animations.Images[0].Metadata.BuildingID)
		}
		// rewind:
		// 	rect, err := packer.Pack(img.Bounds().Dx(), img.Bounds().Dy())
		// 	if err != nil {
		// 		currentAtlasIndex++
		// 		atlas.Images = append(atlas.Images, image.NewRGBA(image.Rect(0, 0, atlasWidth, atlasHeight)))
		// 		packer = NewMaxRectsPacker(atlasWidth, atlasHeight)
		// 		goto rewind
		// 	}
	}
	// 	dstRect := image.Rect(rect.X, rect.Y, rect.X+rect.Width, rect.Y+rect.Height)

	// 	draw.Draw(atlas.Images[currentAtlasIndex], dstRect, img, img.Bounds().Min, draw.Over)
	// 	if atlas.ImagesMeta[currentAtlasIndex] == nil {
	// 		atlas.ImagesMeta[currentAtlasIndex] = map[string]Image{}
	// 	}
	// 	atlas.ImagesMeta[currentAtlasIndex][i] = Image{
	// 		ImageIndex: currentAtlasIndex,
	// 		X:          rect.X,
	// 		Y:          rect.Y,
	// 		Width:      rect.Width,
	// 		Height:     rect.Height,
	// 	}
	// }
	// fmt.Printf("%+v\n", atlas.ImagesMeta)
	return atlas, nil
}

func (a *TextureAtlas) ExportPNG(filename string, img *image.RGBA) error {
	exportPNGFile, err := os.Create(filename)
	if err != nil {
		return err
	}
	defer exportPNGFile.Close()

	if err := png.Encode(exportPNGFile, img); err != nil {
		return err
	}
	return nil
}

// Export saves the texture atlas and its metadata as a JSON file and a PNG file
func (a *TextureAtlas) Export() error {
	if _, err := os.Stat(a.outputDir); os.IsNotExist(err) {
		if err := os.MkdirAll(a.outputDir, os.ModePerm); err != nil {
			return err
		}
	}

	for i, img := range a.Images {
		err := a.ExportPNG(fmt.Sprintf("%s/%s-%04d.png", a.outputDir, a.AtlasMeta.Name, i), img)
		if err != nil {
			return err
		}
	}

	// Save metadata to JSON file
	exportJSONFile, err := os.Create(fmt.Sprintf("%s/%s.json", a.outputDir, a.AtlasMeta.Name))
	if err != nil {
		return err
	}
	defer exportJSONFile.Close()

	j, err := json.MarshalIndent(a, "", "  ")
	if err != nil {
		return err
	}

	_, err = exportJSONFile.Write(j)
	if err != nil {
		return err
	}
	return nil
}

// loadImage loads an image from the specified file path
func loadImage(filename string) (image.Image, error) {
	filename, err := filepath.Abs(filename)
	if err != nil {
		return nil, fmt.Errorf("failed to get absolute path: %v", err)
	}
	file, err := os.Open(filename)
	if err != nil {
		return nil, fmt.Errorf("failed to open image file: %v", err)
	}
	defer file.Close()

	img, err := png.Decode(file)
	if err != nil {
		return nil, fmt.Errorf("failed to decode image: %v", err)
	}
	// if converted, ok := img.(*image.RGBA); ok {
	// 	return converted, nil
	// }

	// return nil, fmt.Errorf("failed to convert image to RGBA")
	return img, nil
}
