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
	"image/color"
	"log"
	"os"
	"path/filepath"
	"time"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/hajimehoshi/ebiten/v2/text"
	"github.com/siredmar/mdcii-engine/pkg/bsh"
	building "github.com/siredmar/mdcii-engine/pkg/building"
	"github.com/siredmar/mdcii-engine/pkg/cod"
	"github.com/siredmar/mdcii-engine/pkg/cod/buildings"
	buildingsCod "github.com/siredmar/mdcii-engine/pkg/cod/buildings"
	"github.com/siredmar/mdcii-engine/pkg/files"
	"github.com/spf13/cobra"
	"golang.org/x/image/font/basicfont"
)

var (
	gamePath      string
	buildingParam int
)

var (
	ScreenWidth  int  = 600
	ScreenHeight int  = 600
	TileSize     int  = 64
	GridEnable   bool = false
)

func init() {
	rootCmd.Flags().StringVarP(&gamePath, "path", "p", ".", "Path to game")
	rootCmd.Flags().IntVarP(&buildingParam, "building", "b", 381, "building")
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

		ebiten.SetWindowSize(ScreenWidth, ScreenHeight)
		ebiten.SetWindowTitle("buildings_gfx")
		game := &Game{
			windowWidth:    ScreenWidth,
			windowHeight:   ScreenHeight,
			tileSize:       TileSize,
			gfxStadtfldBsh: gfxStadtfldBsh,
			buildings:      buildings,
			op:             &ebiten.DrawImageOptions{},
			buffer:         ebiten.NewImage(ScreenWidth, ScreenHeight),
			drawToBuffer:   true,
			Building:       nil,
			buildingId:     buildingParam,
		}
		game.setBuilding(game.buildingId)
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

func (g *Game) setBuilding(id int) {
	b := g.buildings.BuildingsVector[id]
	g.Building = &building.Building{
		BaseIndexSaved:       b.Gfx,
		BaseIndex:            b.Gfx,
		Rotation:             0,
		X:                    ScreenWidth / 4,
		Y:                    ScreenHeight / 4,
		AnimationSteps:       b.AnimationAmount,
		CurrentAnimationStep: 0,
		AnimationAdd:         b.AnimationAdd,
		Size:                 building.BuildingSize(b.Size.W, b.Size.H),
	}
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
	buildingId       int
}

func (g *Game) Draw(screen *ebiten.Image) {
	if g.Building != nil {
		offsets := building.RotationOffsets[g.Building.Size][g.Building.Rotation]
		for i, offset := range offsets {
			screenX := float64(g.Building.X) + float64(offset[0]-offset[1])*(float64(tileWidth)/2)
			screenY := float64(g.Building.Y) + float64(offset[0]+offset[1])*(float64(tileHeight)/2)

			textureKey := calculateTextureKey(g.Building.BaseIndex, g.Building.Rotation, i, g.Building.Size)

			tileImg, ok := g.gfxStadtfldBsh.Images[textureKey]
			if !ok {
				log.Printf("Texture key %s not found in texture atlas", textureKey)
				continue
			}

			baseOffsetY := calculateBaseOffsetY(tileImg.Bounds().Dy(), tileHeight)
			ebitenImg := ebiten.NewImageFromImage(tileImg)

			options := &ebiten.DrawImageOptions{}
			options.GeoM.Translate(screenX, screenY-float64(baseOffsetY))
			options.GeoM.Scale(1.5, 1.5)
			screen.DrawImage(ebitenImg, options)
			g.DrawBuildingInfo(screen)
			g.DrawUsage(screen)
		}
	}
}

func calculateBaseOffsetY(tileHeight, gridTileHeight int) int {
	if tileHeight > gridTileHeight {
		return tileHeight - gridTileHeight
	}
	return 0
}

func calculateTextureKey(baseIndex, rotation, tileIndex int, size building.BuildingSizeIdentifier) string {
	tilesPerRotation := len(building.RotationOffsets[size][rotation])
	return fmt.Sprintf("%d", baseIndex+(rotation*tilesPerRotation)+tileIndex)
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
			g.Building.Rotation = (g.Building.Rotation + 3) % 4 // Linksrotation
			g.lastKeyPressTime = now
		} else if ebiten.IsKeyPressed(ebiten.KeyRight) {
			g.Building.Rotation = (g.Building.Rotation + 1) % 4 // Rechtsrotation
			g.lastKeyPressTime = now
		} else if ebiten.IsKeyPressed(ebiten.KeyUp) {
			if g.Building.AnimationSteps > 0 {
				g.Building.CurrentAnimationStep = (g.Building.CurrentAnimationStep + 1) % g.Building.AnimationSteps
				add := g.Building.CurrentAnimationStep * g.Building.AnimationAdd
				g.Building.BaseIndex = g.Building.BaseIndexSaved + add
			}
			g.lastKeyPressTime = now
		} else if ebiten.IsKeyPressed(ebiten.KeyN) {
			fmt.Println(g.buildingId)
			g.buildingId = (g.buildingId + 1) % len(g.buildings.BuildingsVector)
			fmt.Println(g.buildingId)
			g.setBuilding(g.buildingId)
			g.lastKeyPressTime = now
		} else if ebiten.IsKeyPressed(ebiten.KeyM) {
			if g.buildingId > 0 {
				g.buildingId--
			} else {
				g.buildingId = len(g.buildings.BuildingsVector) - 1
			}
			g.setBuilding(g.buildingId)
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
