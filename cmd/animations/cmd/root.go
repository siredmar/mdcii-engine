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
	"image/draw"
	"log"
	"os"
	"path/filepath"
	"time"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/hajimehoshi/ebiten/v2/text"
	"github.com/siredmar/mdcii-engine/pkg/bsh"
	"github.com/siredmar/mdcii-engine/pkg/cod"
	buildingsCod "github.com/siredmar/mdcii-engine/pkg/cod/buildings"
	"github.com/siredmar/mdcii-engine/pkg/files"
	animations "github.com/siredmar/mdcii-engine/pkg/texture/animations"
	"github.com/siredmar/mdcii-engine/pkg/texture/atlas"
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
	ScreenWidth  int = 1024
	ScreenHeight int = 1024
)

func init() {
	rootCmd.Flags().StringVarP(&gamePath, "path", "p", ".", "Path to game")
	rootCmd.Flags().IntVarP(&buildingIndex, "buildingIndex", "i", 381, "building index")
	// rootCmd.Flags().IntVarP(&buildingParam, "building", "b", 380, "building ID")
}

var rootCmd = &cobra.Command{
	Use:   "animations for buildings",
	Short: "animations for buildings",
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

		atlasWidth := 4096
		atlasHeight := 4096

		atlas, err := atlas.CreateTextureAtlas(atlasWidth, atlasHeight, buildings, atlas.WithName("texture-atlas"), atlas.WithImages(gfxStadtfldBsh))
		if err != nil {
			fmt.Println("Error:", err)
			return
		}
		ani, err := animations.New(atlas)
		if err != nil {
			fmt.Println("Error:", err)
			return
		}

		ebiten.SetWindowSize(ScreenWidth, ScreenHeight)
		ebiten.SetWindowTitle("animations")
		game := &Game{
			windowWidth:  ScreenWidth,
			windowHeight: ScreenHeight,
			animations:   ani,
			// animation:    nil,
			buildingIndex: buildingIndex,
			// buildingId: id,
			count:     0,
			buildings: buildings,
			atlas:     atlas,
		}
		game.buildingId, err = buildings.GetBuildingIdByIndex(game.buildingIndex)
		if err != nil {
			fmt.Println("Error:", err)
			return
		}
		// game.animation = game.animations.GetAnimation(buildingParam, rotation.DEG0)
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

type Game struct {
	windowWidth  int
	windowHeight int
	animations   *animations.Animations
	buildings    *buildingsCod.Buildings
	ScreenWidth  int
	ScreenHeight int
	// buffer           *ebiten.Image
	// op               *ebiten.DrawImageOptions
	// drawToBuffer     bool
	lastKeyPressTime time.Time
	buildingId       int
	buildingIndex    int
	rotation         rotation.Rotation
	// animation        *animations.Animation
	lastTime     time.Time
	count        int
	currentFrame int
	atlas        *atlas.TextureAtlas
	// currentAnimationTime time.Time
}

var once bool

func (g *Game) Draw(screen *ebiten.Image) {
	// i := g.animation.Frames[g.animation.CurrentFrame]
	// g.animation = g.animations.Animations[g.buildingId][g.rotation]
	// if g.animation.Animated {
	// 	if g.animation.Started {
	// 		if g.animation.Once {
	// 			if g.animation.CurrentFrame < g.animation.Steps {
	// 				g.animation.CurrentFrame++
	// 			}
	// 		} else {
	// g.animation.CurrentFrame = (g.animation.CurrentFrame + 1) % g.animation.Steps
	// if g.animation != nil {
	// g.currentFrame = (g.count / 5) % g.animation.Steps
	// }

	// ebitenImg := ebiten.NewImageFromImage(
	options := &ebiten.DrawImageOptions{}
	options.GeoM.Translate(100, 100)
	// options.GeoM.Scale(1.5, 1.5)
	frame := g.animations.Animations[g.buildingId][g.rotation].Frames[g.currentFrame]
	if !once {
		g.atlas.ExportPNG("out.png", toRGBA(*frame))
		once = true
	}
	ebitemImg := ebiten.NewImageFromImage(*frame)
	screen.DrawImage(ebitemImg, options)
	// }
	// g.DrawBuildingInfo(screen)
	// g.DrawUsage(screen)
	// }
	// }
}

func toRGBA(src image.Image) *image.RGBA {
	// Get the bounds of the source image
	bounds := src.Bounds()

	// Create a new RGBA image with the same bounds
	rgba := image.NewRGBA(bounds)

	// Draw the source image onto the RGBA image
	draw.Draw(rgba, bounds, src, bounds.Min, draw.Src)

	return rgba
}

func (g *Game) DrawBuildingInfo(screen *ebiten.Image) {
	textColor := color.RGBA{255, 255, 255, 255}
	face := basicfont.Face7x13

	b := g.buildings.BuildingsVector[g.buildingId]
	text.Draw(screen, fmt.Sprintf("Building Id: %d", b.Id), face, 10, 10, textColor)
	text.Draw(screen, fmt.Sprintf("Building Size: %d,%d", b.Size.W, b.Size.H), face, 10, 20, textColor)
	text.Draw(screen, fmt.Sprintf("Building Animation Amount: %d", b.AnimationAmount), face, 10, 30, textColor)
	text.Draw(screen, fmt.Sprintf("Building Animation Add: %d", b.AnimationAdd), face, 10, 40, textColor)
	text.Draw(screen, fmt.Sprintf("Building Gfx: %d", b.Gfx), face, 10, 50, textColor)
	// text.Draw(screen, fmt.Sprintf("Building Rotation: %d", g.Building.Rotation), face, 10, 60, textColor)
	// text.Draw(screen, fmt.Sprintf("Building BaseIndex: %d", g.Building.BaseIndex), face, 10, 70, textColor)
	// text.Draw(screen, fmt.Sprintf("CurrentAnimationStep: %d", g.Building.CurrentAnimationStep), face, 10, 80, textColor)
}

func (g *Game) DrawUsage(screen *ebiten.Image) {
	textColor := color.RGBA{255, 255, 255, 255}
	face := basicfont.Face7x13

	text.Draw(screen, "Up: Animation Step, Left/Right: Rotate, N: next, M: previous", face, 10, ScreenHeight-20, textColor)

}

func (g *Game) Update() error {
	g.count++

	const debounceDuration = time.Millisecond * 100
	// if g.animation == nil {
	// 	return nil
	// }
	now := time.Now()
	// if time.Duration(g.lastTime.Sub(now)) >= g.animation.FrameDuration {
	// 	g.animation.CurrentFrame = (g.animation.CurrentFrame + 1) % g.animation.Steps
	// }

	if now.Sub(g.lastKeyPressTime) >= debounceDuration {
		if ebiten.IsKeyPressed(ebiten.KeyLeft) {
			g.rotation.Increment()
			// g.animation = g.animations.GetAnimation(g.buildingId, g.rotation)
			g.lastKeyPressTime = now

		} else if ebiten.IsKeyPressed(ebiten.KeyRight) {
			g.rotation.Decrement()
			// g.animation = g.animations.GetAnimation(g.buildingId, g.rotation)
			g.lastKeyPressTime = now
		} else if ebiten.IsKeyPressed(ebiten.KeyN) {
			fmt.Println(g.buildingId)
			g.buildingIndex = (g.buildingIndex + 1) % len(g.buildings.BuildingsVector)
			var err error
			g.buildingId, err = g.buildings.GetBuildingIdByIndex(g.buildingIndex)
			if err != nil {
				log.Fatalf("Error:", err)
			}
			fmt.Println(g.buildingId)
			// g.animation = g.animations.GetAnimation(g.buildingId, g.rotation)
			g.lastKeyPressTime = now
		} else if ebiten.IsKeyPressed(ebiten.KeyM) {
			if g.buildingIndex > 0 {
				g.buildingIndex--
			} else {
				g.buildingIndex = len(g.buildings.BuildingsVector) - 1
			}
			var err error
			g.buildingId, err = g.buildings.GetBuildingIdByIndex(g.buildingIndex)
			if err != nil {
				log.Fatalf("Error:", err)
			}
			// g.animation = g.animations.GetAnimation(g.buildingId, g.rotation)

			g.lastKeyPressTime = now
		} else if ebiten.IsKeyPressed(ebiten.KeyEscape) {
			os.Exit(0)
		}
	}
	g.lastTime = now
	return nil
}

func (g *Game) Layout(outsideWidth, outsideHeight int) (int, int) {
	return ScreenWidth, ScreenHeight
}
