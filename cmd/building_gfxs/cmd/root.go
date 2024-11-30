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
	building := g.buildings.BuildingsVector[id]
	g.Building = &Building{
		BaseIndexSaved:       building.Gfx,
		BaseIndex:            building.Gfx,
		Rotation:             0,
		X:                    ScreenWidth / 4,
		Y:                    ScreenHeight / 4,
		AnimationSteps:       building.AnimationAmount,
		CurrentAnimationStep: 0,
		AnimationAdd:         building.AnimationAdd,
		Size:                 buildingSize(building.Size.W, building.Size.H),
	}
}

// Building enthält die Informationen eines Gebäudes
type Building struct {
	BaseIndexSaved       int
	BaseIndex            int
	Rotation             int
	X, Y                 int
	AnimationSteps       int
	CurrentAnimationStep int
	AnimationAdd         int
	Size                 BuildingSizeIdentifier
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
	Building         *Building
	lastKeyPressTime time.Time
	buildingId       int
}

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

func buildingSize(w, h int) BuildingSizeIdentifier {
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

var (
	rotationOffsets = map[BuildingSizeIdentifier][][][]int{
		BuildingSize1x1: {
			{{0, 0}}, // Rotation 0
			{{0, 0}}, // Rotation 1
			{{0, 0}}, // Rotation 2
			{{0, 0}}, // Rotation 3
		},
		BuildingSize2x2: {
			{{0, 0}, {1, 0}, {0, 1}, {1, 1}}, // Rotation 0
			{{1, 0}, {1, 1}, {0, 0}, {0, 1}}, // Rotation 1
			{{1, 1}, {0, 1}, {1, 0}, {0, 0}}, // Rotation 2
			{{0, 1}, {0, 0}, {1, 1}, {1, 0}}, // Rotation 3
		},
		BuildingSize2x3: {
			{{0, 0}, {1, 0}, {0, 1}, {1, 1}, {0, 2}, {1, 2}}, // Rotation 0
			{{2, 0}, {2, 1}, {1, 0}, {1, 1}, {0, 0}, {0, 1}}, // Rotation 1
			{{1, 2}, {0, 2}, {1, 1}, {0, 1}, {1, 0}, {0, 0}}, // Rotation 2
			{{0, 1}, {0, 0}, {1, 1}, {1, 0}, {2, 1}, {2, 0}}, // Rotation 3
		},
		BuildingSize2x1: {
			{{0, 0}, {1, 0}}, // Rotation 0: horizontal (2 wide, 1 tall)
			{{1, 0}, {1, 1}}, // Rotation 1: vertical (1 wide, 2 tall)
			{{1, 1}, {0, 1}}, // Rotation 2: horizontal flipped (2 wide, 1 tall)
			{{0, 1}, {0, 0}}, // Rotation 3: vertical flipped (1 wide, 2 tall)
		},
		BuildingSize1x2: {
			{{0, 0}, {0, 1}}, // Rotation 0
			{{1, 1}, {0, 1}}, // Rotation 1
			{{0, 1}, {0, 0}}, // Rotation 2
			{{0, 1}, {1, 1}}, // Rotation 3
		},
		BuildingSize1x3: {
			{{0, 0}, {0, 1}, {0, 2}}, // Rotation 0
			{{2, 0}, {1, 0}, {0, 0}}, // Rotation 1
			{{0, 2}, {0, 1}, {0, 0}}, // Rotation 2
			{{0, 0}, {1, 0}, {2, 0}}, // Rotation 3
		},
		BuildingSize3x3: {
			{{0, 0}, {1, 0}, {2, 0}, {0, 1}, {1, 1}, {2, 1}, {0, 2}, {1, 2}, {2, 2}}, // Rotation 0
			{{2, 0}, {2, 1}, {2, 2}, {1, 0}, {1, 1}, {1, 2}, {0, 0}, {0, 1}, {0, 2}}, // Rotation 1
			{{2, 2}, {1, 2}, {0, 2}, {2, 1}, {1, 1}, {0, 1}, {2, 0}, {1, 0}, {0, 0}}, // Rotation 2
			{{0, 2}, {0, 1}, {0, 0}, {1, 2}, {1, 1}, {1, 0}, {2, 2}, {2, 1}, {2, 0}}, // Rotation 3
		},
		BuildingSize4x3: {
			{{0, 0}, {1, 0}, {2, 0}, {3, 0}, {0, 1}, {1, 1}, {2, 1}, {3, 1}, {0, 2}, {1, 2}, {2, 2}, {3, 2}}, // Rotation 0: 4 wide, 3 tall
			{{2, 0}, {2, 1}, {2, 2}, {2, 3}, {1, 0}, {1, 1}, {1, 2}, {1, 3}, {0, 0}, {0, 1}, {0, 2}, {0, 3}}, // Rotation 1: 3 wide, 4 tall (rotated 90° clockwise)
			{{3, 2}, {2, 2}, {1, 2}, {0, 2}, {3, 1}, {2, 1}, {1, 1}, {0, 1}, {3, 0}, {2, 0}, {1, 0}, {0, 0}}, // Rotation 2: 4 wide, 3 tall (rotated 180°)
			{{0, 3}, {0, 2}, {0, 1}, {0, 0}, {1, 3}, {1, 2}, {1, 1}, {1, 0}, {2, 3}, {2, 2}, {2, 1}, {2, 0}}, // Rotation 3: 3 wide, 4 tall (rotated 270° clockwise)
		},
		BuildingSize4x4: {
			{{0, 0}, {1, 0}, {2, 0}, {3, 0}, {0, 1}, {1, 1}, {2, 1}, {3, 1}, {0, 2}, {1, 2}, {2, 2}, {3, 2}, {0, 3}, {1, 3}, {2, 3}, {3, 3}}, // Rotation 0
			{{3, 0}, {3, 1}, {3, 2}, {3, 3}, {2, 0}, {2, 1}, {2, 2}, {2, 3}, {1, 0}, {1, 1}, {1, 2}, {1, 3}, {0, 0}, {0, 1}, {0, 2}, {0, 3}}, // Rotation 1
			{{3, 3}, {2, 3}, {1, 3}, {0, 3}, {3, 2}, {2, 2}, {1, 2}, {0, 2}, {3, 1}, {2, 1}, {1, 1}, {0, 1}, {3, 0}, {2, 0}, {1, 0}, {0, 0}}, // Rotation 2
			{{0, 3}, {0, 2}, {0, 1}, {0, 0}, {1, 3}, {1, 2}, {1, 1}, {1, 0}, {2, 3}, {2, 2}, {2, 1}, {2, 0}, {3, 3}, {3, 2}, {3, 1}, {3, 0}}, // Rotation 3
		},
		BuildingSize5x5: {
			{{0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {0, 1}, {1, 1}, {2, 1}, {3, 1}, {4, 1}, {0, 2}, {1, 2}, {2, 2}, {3, 2}, {4, 2}, {0, 3}, {1, 3}, {2, 3}, {3, 3}, {4, 3}, {0, 4}, {1, 4}, {2, 4}, {3, 4}, {4, 4}}, // Rotation 0
			{{4, 0}, {4, 1}, {4, 2}, {4, 3}, {4, 4}, {3, 0}, {3, 1}, {3, 2}, {3, 3}, {3, 4}, {2, 0}, {2, 1}, {2, 2}, {2, 3}, {2, 4}, {1, 0}, {1, 1}, {1, 2}, {1, 3}, {1, 4}, {0, 0}, {0, 1}, {0, 2}, {0, 3}, {0, 4}}, // Rotation 1
			{{4, 4}, {3, 4}, {2, 4}, {1, 4}, {0, 4}, {4, 3}, {3, 3}, {2, 3}, {1, 3}, {0, 3}, {4, 2}, {3, 2}, {2, 2}, {1, 2}, {0, 2}, {4, 1}, {3, 1}, {2, 1}, {1, 1}, {0, 1}, {4, 0}, {3, 0}, {2, 0}, {1, 0}, {0, 0}}, // Rotation 2
			{{0, 4}, {0, 3}, {0, 2}, {0, 1}, {0, 0}, {1, 4}, {1, 3}, {1, 2}, {1, 1}, {1, 0}, {2, 4}, {2, 3}, {2, 2}, {2, 1}, {2, 0}, {3, 4}, {3, 3}, {3, 2}, {3, 1}, {3, 0}, {4, 4}, {4, 3}, {4, 2}, {4, 1}, {4, 0}}, // Rotation 3
		},
		BuildingSize6x6: {
			{{0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, {0, 1}, {1, 1}, {2, 1}, {3, 1}, {4, 1}, {5, 1}, {0, 2}, {1, 2}, {2, 2}, {3, 2}, {4, 2}, {5, 2}, {0, 3}, {1, 3}, {2, 3}, {3, 3}, {4, 3}, {5, 3}, {0, 4}, {1, 4}, {2, 4}, {3, 4}, {4, 4}, {5, 4}, {0, 5}, {1, 5}, {2, 5}, {3, 5}, {4, 5}, {5, 5}}, // Rotation 0
			{{5, 0}, {5, 1}, {5, 2}, {5, 3}, {5, 4}, {5, 5}, {4, 0}, {4, 1}, {4, 2}, {4, 3}, {4, 4}, {4, 5}, {3, 0}, {3, 1}, {3, 2}, {3, 3}, {3, 4}, {3, 5}, {2, 0}, {2, 1}, {2, 2}, {2, 3}, {2, 4}, {2, 5}, {1, 0}, {1, 1}, {1, 2}, {1, 3}, {1, 4}, {1, 5}, {0, 0}, {0, 1}, {0, 2}, {0, 3}, {0, 4}, {0, 5}}, // Rotation 1
			{{5, 5}, {4, 5}, {3, 5}, {2, 5}, {1, 5}, {0, 5}, {5, 4}, {4, 4}, {3, 4}, {2, 4}, {1, 4}, {0, 4}, {5, 3}, {4, 3}, {3, 3}, {2, 3}, {1, 3}, {0, 3}, {5, 2}, {4, 2}, {3, 2}, {2, 2}, {1, 2}, {0, 2}, {5, 1}, {4, 1}, {3, 1}, {2, 1}, {1, 1}, {0, 1}, {5, 0}, {4, 0}, {3, 0}, {2, 0}, {1, 0}, {0, 0}}, // Rotation 2
			{{0, 5}, {0, 4}, {0, 3}, {0, 2}, {0, 1}, {0, 0}, {1, 5}, {1, 4}, {1, 3}, {1, 2}, {1, 1}, {1, 0}, {2, 5}, {2, 4}, {2, 3}, {2, 2}, {2, 1}, {2, 0}, {3, 5}, {3, 4}, {3, 3}, {3, 2}, {3, 1}, {3, 0}, {4, 5}, {4, 4}, {4, 3}, {4, 2}, {4, 1}, {4, 0}, {5, 5}, {5, 4}, {5, 3}, {5, 2}, {5, 1}, {5, 0}}, // Rotation 3
		},
		BuildingSize6x4: {
			{{0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, {0, 1}, {1, 1}, {2, 1}, {3, 1}, {4, 1}, {5, 1}, {0, 2}, {1, 2}, {2, 2}, {3, 2}, {4, 2}, {5, 2}, {0, 3}, {1, 3}, {2, 3}, {3, 3}, {4, 3}, {5, 3}}, // Rotation 0
			{{3, 0}, {3, 1}, {3, 2}, {3, 3}, {3, 4}, {3, 5}, {2, 0}, {2, 1}, {2, 2}, {2, 3}, {2, 4}, {2, 5}, {1, 0}, {1, 1}, {1, 2}, {1, 3}, {1, 4}, {1, 5}, {0, 0}, {0, 1}, {0, 2}, {0, 3}, {0, 4}, {0, 5}}, // Rotation 1
			{{5, 3}, {4, 3}, {3, 3}, {2, 3}, {1, 3}, {0, 3}, {5, 2}, {4, 2}, {3, 2}, {2, 2}, {1, 2}, {0, 2}, {5, 1}, {4, 1}, {3, 1}, {2, 1}, {1, 1}, {0, 1}, {5, 0}, {4, 0}, {3, 0}, {2, 0}, {1, 0}, {0, 0}}, // Rotation 2
			{{0, 5}, {0, 4}, {0, 3}, {0, 2}, {0, 1}, {0, 0}, {1, 5}, {1, 4}, {1, 3}, {1, 2}, {1, 1}, {1, 0}, {2, 5}, {2, 4}, {2, 3}, {2, 2}, {2, 1}, {2, 0}, {3, 5}, {3, 4}, {3, 3}, {3, 2}, {3, 1}, {3, 0}}, // Rotation 3
		},
		BuildingSize5x7: {
			// Rotation 0: 5 wide, 7 tall
			{
				{0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0},
				{0, 1}, {1, 1}, {2, 1}, {3, 1}, {4, 1},
				{0, 2}, {1, 2}, {2, 2}, {3, 2}, {4, 2},
				{0, 3}, {1, 3}, {2, 3}, {3, 3}, {4, 3},
				{0, 4}, {1, 4}, {2, 4}, {3, 4}, {4, 4},
				{0, 5}, {1, 5}, {2, 5}, {3, 5}, {4, 5},
				{0, 6}, {1, 6}, {2, 6}, {3, 6}, {4, 6},
			},
			// Rotation 1: 7 wide, 5 tall (rotated 90° clockwise)
			{
				{6, 0}, {6, 1}, {6, 2}, {6, 3}, {6, 4},
				{5, 0}, {5, 1}, {5, 2}, {5, 3}, {5, 4},
				{4, 0}, {4, 1}, {4, 2}, {4, 3}, {4, 4},
				{3, 0}, {3, 1}, {3, 2}, {3, 3}, {3, 4},
				{2, 0}, {2, 1}, {2, 2}, {2, 3}, {2, 4},
				{1, 0}, {1, 1}, {1, 2}, {1, 3}, {1, 4},
				{0, 0}, {0, 1}, {0, 2}, {0, 3}, {0, 4},
			},
			// Rotation 2: 5 wide, 7 tall (rotated 180°)
			{
				{4, 6}, {3, 6}, {2, 6}, {1, 6}, {0, 6},
				{4, 5}, {3, 5}, {2, 5}, {1, 5}, {0, 5},
				{4, 4}, {3, 4}, {2, 4}, {1, 4}, {0, 4},
				{4, 3}, {3, 3}, {2, 3}, {1, 3}, {0, 3},
				{4, 2}, {3, 2}, {2, 2}, {1, 2}, {0, 2},
				{4, 1}, {3, 1}, {2, 1}, {1, 1}, {0, 1},
				{4, 0}, {3, 0}, {2, 0}, {1, 0}, {0, 0},
			},
			// Rotation 3: 7 wide, 5 tall (rotated 270° clockwise)
			{
				{0, 4}, {0, 3}, {0, 2}, {0, 1}, {0, 0},
				{1, 4}, {1, 3}, {1, 2}, {1, 1}, {1, 0},
				{2, 4}, {2, 3}, {2, 2}, {2, 1}, {2, 0},
				{3, 4}, {3, 3}, {3, 2}, {3, 1}, {3, 0},
				{4, 4}, {4, 3}, {4, 2}, {4, 1}, {4, 0},
				{5, 4}, {5, 3}, {5, 2}, {5, 1}, {5, 0},
				{6, 4}, {6, 3}, {6, 2}, {6, 1}, {6, 0},
			},
		},
	}
)

func (g *Game) Draw(screen *ebiten.Image) {
	if g.Building != nil {
		offsets := rotationOffsets[g.Building.Size][g.Building.Rotation]
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

func calculateTextureKey(baseIndex, rotation, tileIndex int, size BuildingSizeIdentifier) string {
	tilesPerRotation := len(rotationOffsets[size][rotation])
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
