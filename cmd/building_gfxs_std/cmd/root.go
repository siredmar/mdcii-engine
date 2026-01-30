/*
Copyright © 2023 NAME HERE <EMAIL ADDRESS>

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.

	See the License for the specific language governing permissions and

limitations under the License.
*/
package cmd

import (
	"fmt"
	"image"
	"image/draw"
	"image/png"
	"log"
	"os"
	"path/filepath"

	"github.com/siredmar/mdcii-engine/pkg/bsh"
	building "github.com/siredmar/mdcii-engine/pkg/building"
	"github.com/siredmar/mdcii-engine/pkg/cod"
	buildingsCod "github.com/siredmar/mdcii-engine/pkg/cod/buildings"
	"github.com/siredmar/mdcii-engine/pkg/files"
	"github.com/spf13/cobra"
)

var (
	gamePath      string
	buildingParam int
	ScreenWidth   int = 600
	ScreenHeight  int = 600
	TileSize      int = 64
)

func init() {
	rootCmd.Flags().StringVarP(&gamePath, "path", "p", ".", "Path to game")
	rootCmd.Flags().IntVarP(&buildingParam, "building", "b", 381, "building ID")
}

const (
	tileWidth  = 64 // Tile width in pixels
	tileHeight = 31 // Tile height in pixels
)

var rootCmd = &cobra.Command{
	Use:   "gfx viewer for buildings",
	Short: "gfx viewer for buildings",
	Run: func(cmd *cobra.Command, args []string) {
		absPath, err := filepath.Abs(gamePath)
		if err != nil {
			fmt.Println(err)
			os.Exit(1)
		}

		dirPath := filepath.Dir(absPath)

		files.CreateInstance(dirPath)
		buildingsCodPath, err := files.Instance().FindPathForFile("haeuser.cod")
		if err != nil {
			fmt.Println(err)
			os.Exit(1)
		}
		haeuserCod, err := cod.NewCod(buildingsCodPath, true)
		if err != nil {
			fmt.Println(err)
			os.Exit(1)
		}
		err = haeuserCod.Parse()
		if err != nil {
			fmt.Println(err)
			os.Exit(1)
		}

		buildings, err := buildingsCod.NewBuildings(haeuserCod)
		if err != nil {
			fmt.Println(err)
			os.Exit(1)
		}

		gfxStadtfldBshPath, err := files.Instance().FindPathForFile("gfx/stadtfld.bsh")
		if err != nil {
			fmt.Println(err)
			os.Exit(1)
		}
		gfxStadtfldBsh, err := bsh.NewPng(bsh.WithFile(gfxStadtfldBshPath), bsh.WithConvertAll())
		if err != nil {
			fmt.Println(err)
			os.Exit(1)
		}

		renderToPNG(ScreenWidth, ScreenHeight, buildings, gfxStadtfldBsh, buildingParam)
	},
}

func renderToPNG(screenWidth, screenHeight int, buildings *buildingsCod.Buildings, gfxStadtfldBsh *bsh.BshPng, buildingID int) {
	// Create a blank RGBA image for drawing
	outputImage := image.NewRGBA(image.Rect(0, 0, screenWidth, screenHeight))

	// Fetch the building data
	b := buildings.BuildingsVector[buildingID]
	buildingData := &building.Building{
		BaseIndex: b.Gfx,
		Size:      building.BuildingSize(b.Size.W, b.Size.H),
		Rotation:  0,
		X:         screenWidth / 4,
		Y:         screenHeight / 4,
	}

	// Draw the building
	offsets := building.GenerateTileOffsets(buildingData.Size.Width(), buildingData.Size.Height(), buildingData.Rotation)
	for i, offset := range offsets {
		screenX := buildingData.X + (offset[0]-offset[1])*(tileWidth/2)
		screenY := buildingData.Y + (offset[0]+offset[1])*(tileHeight/2)

		textureKey := fmt.Sprintf("%d", buildingData.BaseIndex+i)
		tileImg, ok := gfxStadtfldBsh.Images[textureKey]
		if !ok {
			log.Printf("Texture key %s not found in texture atlas", textureKey)
			continue
		}

		baseOffsetY := 0
		if tileImg.Bounds().Dy() > tileHeight {
			baseOffsetY = tileImg.Bounds().Dy() - tileHeight
		}

		draw.Draw(outputImage, image.Rect(screenX, screenY-baseOffsetY, screenX+tileWidth, screenY+tileHeight),
			tileImg, image.Point{}, draw.Over)
	}

	// Find the bounds of the non-alpha content
	cropBounds := findNonAlphaBounds(outputImage)

	// Crop the image to the determined bounds
	croppedImage := cropImage(outputImage, cropBounds)

	// Save the cropped output image as a PNG file
	outputFile, err := os.Create("output.png")
	if err != nil {
		log.Fatal(err)
	}
	defer outputFile.Close()

	err = png.Encode(outputFile, croppedImage)
	if err != nil {
		log.Fatal(err)
	}
	fmt.Println("Image rendered, cropped, and saved as output.png")
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

func Execute() {
	if err := rootCmd.Execute(); err != nil {
		fmt.Println(err)
		os.Exit(1)
	}
}
