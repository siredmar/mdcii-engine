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
	"image/color"
	"log"
	"os"
	"path/filepath"
	"time"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/hajimehoshi/ebiten/v2/text"
	"github.com/siredmar/mdcii-engine/pkg/bsh"
	"github.com/siredmar/mdcii-engine/pkg/building"
	"github.com/siredmar/mdcii-engine/pkg/cod"
	"github.com/siredmar/mdcii-engine/pkg/cod/buildings"
	buildingsCod "github.com/siredmar/mdcii-engine/pkg/cod/buildings"
	"github.com/siredmar/mdcii-engine/pkg/files"
	atlas "github.com/siredmar/mdcii-engine/pkg/texture/atlas"
	"github.com/siredmar/mdcii-engine/pkg/world/rotation"
	"github.com/spf13/cobra"
	"golang.org/x/image/font/basicfont"
)

var (
	gamePath      string
	buildingIndex int
	// buildingParam int
)

var (
	ScreenWidth  int  = 600
	ScreenHeight int  = 600
	TileSize     int  = 64
	GridEnable   bool = false
)

func init() {
	rootCmd.Flags().StringVarP(&gamePath, "path", "p", ".", "Path to game")
	rootCmd.Flags().IntVarP(&buildingIndex, "buildingIndex", "i", 381, "building index")
	// rootCmd.Flags().IntVarP(&buildingParam, "building", "b", 2121, "building ID")
}

const (
	tileWidth  = 64 // Breite eines Tiles in Pixel
	tileHeight = 31 // Höhe eines Tiles in Pixel
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

		atlas, err := atlas.New(4096, 4096, buildings, atlas.WithName("texture-atlas"), atlas.WithImages(gfxStadtfldBsh))
		if err != nil {
			fmt.Println(err)
			os.Exit(1)
		}

		// if buildingIndex == -1 {
		// 	for i, b := range buildings.BuildingsVector {
		// 		if b.Id == buildingParam {
		// 			buildingIndex = i
		// 			break
		// 		}
		// 	}
		// }

		ebiten.SetWindowSize(ScreenWidth, ScreenHeight)
		ebiten.SetWindowTitle("building_from_atlas")
		game := &Game{
			windowWidth:    ScreenWidth,
			windowHeight:   ScreenHeight,
			tileSize:       TileSize,
			gfxStadtfldBsh: gfxStadtfldBsh,
			buildings:      buildings,
			op:             &ebiten.DrawImageOptions{},
			buffer:         ebiten.NewImage(ScreenWidth, ScreenHeight),
			drawToBuffer:   true,
			Building: &building.Building{
				AnimationSteps:       0,
				CurrentAnimationStep: 0,
			},
			buildingIndex:  buildingIndex,
			atlas:          atlas,
			Sprite:         nil,
			rotation:       rotation.DEG0,
			animationIndex: 0,
		}
		game.setBuilding(game.buildingIndex, 0)
		if err := ebiten.RunGame(game); err != nil {
			log.Fatal(err)
		}
	},
}

func Execute() {
	if err := rootCmd.Execute(); err != nil {
		fmt.Println(err)
		os.Exit(1)
	}

}

func (g *Game) setBuilding(index int, animationStep int) {
	fmt.Println("Building ID", index)
	fmt.Println("Rotation", g.rotation)
	fmt.Println("Animation Index", animationStep)
	b := g.buildings.BuildingsVector[index]
	sprite := g.atlas.ImagesMeta[b.Id].Animations[g.rotation].Images[animationStep].Sprite

	g.Building.Sprite = sprite
	g.Building.AnimationSteps = b.AnimationAmount
	g.Building.CurrentAnimationStep = animationStep
	g.Building.Size = building.BuildingSize(b.Size.W, b.Size.H)
}

type Game struct {
	windowWidth      int
	windowHeight     int
	tileSize         int
	gfxStadtfldBsh   *bsh.BshPng
	buildings        *buildings.Buildings
	ScreenWidth      int
	ScreenHeight     int
	buffer           *ebiten.Image
	op               *ebiten.DrawImageOptions
	drawToBuffer     bool
	Building         *building.Building
	lastKeyPressTime time.Time
	buildingIndex    int
	atlas            *atlas.TextureAtlas
	Sprite           image.Image
	rotation         rotation.Rotation
	animationIndex   int
}

func (g *Game) Draw(screen *ebiten.Image) {

	ebitenImg := ebiten.NewImageFromImage(g.Building.Sprite)

	options := &ebiten.DrawImageOptions{}
	options.GeoM.Translate(100, 100)
	options.GeoM.Scale(1.5, 1.5)
	screen.DrawImage(ebitenImg, options)
	g.DrawBuildingInfo(screen)
	g.DrawUsage(screen)
}

func (g *Game) DrawBuildingInfo(screen *ebiten.Image) {
	textColor := color.RGBA{255, 255, 255, 255}
	face := basicfont.Face7x13

	b := g.buildings.BuildingsVector[g.buildingIndex]
	text.Draw(screen, fmt.Sprintf("Building Id: %d", b.Id), face, 10, 10, textColor)
	text.Draw(screen, fmt.Sprintf("Building Size: %d,%d", b.Size.W, b.Size.H), face, 10, 20, textColor)
	text.Draw(screen, fmt.Sprintf("Building Animation Amount: %d", b.AnimationAmount), face, 10, 30, textColor)
	text.Draw(screen, fmt.Sprintf("Building Animation Add: %d", b.AnimationAdd), face, 10, 40, textColor)
	text.Draw(screen, fmt.Sprintf("Building Gfx: %d", b.Gfx), face, 10, 50, textColor)
	text.Draw(screen, fmt.Sprintf("Building Rotation: %d", g.Building.Rotation), face, 10, 60, textColor)
	text.Draw(screen, fmt.Sprintf("Building BaseIndex: %d", g.Building.BaseIndex), face, 10, 70, textColor)
	text.Draw(screen, fmt.Sprintf("CurrentAnimationStep: %d", g.Building.CurrentAnimationStep), face, 10, 80, textColor)
}

func (g *Game) DrawUsage(screen *ebiten.Image) {
	textColor := color.RGBA{255, 255, 255, 255}
	face := basicfont.Face7x13

	text.Draw(screen, "Up: Animation Step, Left/Right: Rotate, N: next, M: previous", face, 10, ScreenHeight-20, textColor)

}

func (g *Game) Update() error {
	const debounceDuration = time.Millisecond * 100

	now := time.Now()

	if now.Sub(g.lastKeyPressTime) >= debounceDuration {
		if ebiten.IsKeyPressed(ebiten.KeyLeft) {
			// g.Building.Rotation = (g.Building.Rotation + 3) % 4 // Linksrotation
			g.rotation.Decrement()
			g.setBuilding(g.buildingIndex, g.animationIndex)
			g.lastKeyPressTime = now
		} else if ebiten.IsKeyPressed(ebiten.KeyRight) {
			// g.Building.Rotation = (g.Building.Rotation + 1) % 4 // Rechtsrotation
			g.rotation.Increment()
			g.setBuilding(g.buildingIndex, g.animationIndex)
			g.lastKeyPressTime = now
		} else if ebiten.IsKeyPressed(ebiten.KeyUp) {
			fmt.Println(g.Building.AnimationSteps)
			if g.Building.AnimationSteps > 0 {
				g.Building.CurrentAnimationStep = (g.Building.CurrentAnimationStep + 1) % g.Building.AnimationSteps
				// add := g.Building.CurrentAnimationStep * g.Building.AnimationAdd
				// g.Building.BaseIndex = g.Building.BaseIndexSaved + add
				// g.Building.CurrentAnimationStep++
				// g.Building.CurrentAnimationStep = g.Building.CurrentAnimationStep % g.Building.AnimationSteps
				g.setBuilding(g.buildingIndex, g.Building.CurrentAnimationStep)
			}
			g.lastKeyPressTime = now
		} else if ebiten.IsKeyPressed(ebiten.KeyN) {
			fmt.Println(g.buildingIndex)
			g.buildingIndex = (g.buildingIndex + 1) % len(g.buildings.BuildingsVector)
			fmt.Println(g.buildingIndex)
			g.setBuilding(g.buildingIndex, g.animationIndex)
			g.lastKeyPressTime = now
		} else if ebiten.IsKeyPressed(ebiten.KeyM) {
			if g.buildingIndex > 0 {
				g.buildingIndex--
			} else {
				g.buildingIndex = len(g.buildings.BuildingsVector) - 1
			}
			g.setBuilding(g.buildingIndex, g.animationIndex)
			g.lastKeyPressTime = now
		} else if ebiten.IsKeyPressed(ebiten.KeyEscape) {
			os.Exit(0)
		}
	}
	return nil
}

func (g *Game) Layout(outsideWidth, outsideHeight int) (int, int) {
	return ScreenWidth, ScreenHeight
}
