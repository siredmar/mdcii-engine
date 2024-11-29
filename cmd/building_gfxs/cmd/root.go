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
	"log"
	"os"
	"path/filepath"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/siredmar/mdcii-engine/pkg/bsh"
	"github.com/siredmar/mdcii-engine/pkg/cod"
	"github.com/siredmar/mdcii-engine/pkg/cod/buildings"
	buildingsCod "github.com/siredmar/mdcii-engine/pkg/cod/buildings"
	"github.com/siredmar/mdcii-engine/pkg/files"
	"github.com/siredmar/mdcii-engine/pkg/texture/mapping"
	"github.com/spf13/cobra"
)

var (
	gamePath string
	// gamFile  string
	building      int
	rotation      int
	animationStep int
)

var (
	ScreenWidth  int  = 450
	ScreenHeight int  = 500
	TileSize     int  = 64
	GridEnable   bool = false
)

var rootCmd = &cobra.Command{
	Use:   "building_gfxs",
	Short: "generate ",
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

		building := buildings.Buildings[building]
		m, err := mapping.Generate(building)
		if err != nil {
			fmt.Println(err)
			os.Exit(1)
		}

		fmt.Println("building", building)
		fmt.Println("rotation", rotation)
		fmt.Println("step", animationStep)
		fmt.Println("mapping", m)

		ebiten.SetWindowSize(ScreenWidth, ScreenHeight)
		ebiten.SetWindowTitle("islandpng")
		game := &Game{
			windowWidth:    ScreenWidth,
			windowHeight:   ScreenHeight,
			tileSize:       TileSize,
			gfxStadtfldBsh: gfxStadtfldBsh,
			building:       building,
			step:           animationStep,
			rotation:       rotation,
			buildings:      buildings,
			mapping:        m,
			op:             &ebiten.DrawImageOptions{},
			buffer:         ebiten.NewImage(ScreenWidth, ScreenHeight),
			drawToBuffer:   true,

			Building: &Building{
				BaseIndex: building.Gfx, // Basisindex im Texture-Atlas (z. B. Key im Images-Map)
				Rotation:  rotation,     // Startrotation
				X:         100,          // Startposition auf dem Bildschirm
				Y:         150,
			},
		}

		if err := ebiten.RunGame(game); err != nil {
			log.Fatal(err)
		}
	},
}

// Building enthält die Informationen eines Gebäudes
type Building struct {
	BaseIndex int // Startindex im Texture-Atlas (Key in der Map)
	Rotation  int // Aktuelle Rotation (0–3)
	X, Y      int // Startposition auf dem Bildschirm
}

type Game struct {
	windowWidth    int
	windowHeight   int
	tileSize       int
	building       *buildings.Building
	step           int
	rotation       int
	mapping        *mapping.Building
	gfxStadtfldBsh *bsh.BshPng
	buildings      *buildings.Buildings
	ScreenWidth    int
	ScreenHeight   int
	buffer         *ebiten.Image
	op             *ebiten.DrawImageOptions
	drawToBuffer   bool
	Building       *Building
}

// Execute adds all child commands to the root command and sets flags appropriately.
// This is called by main.main(). It only needs to happen once to the rootCmd.
func Execute() {
	if err := rootCmd.Execute(); err != nil {
		fmt.Println(err)
		os.Exit(1)
	}

}

func init() {
	rootCmd.Flags().StringVarP(&gamePath, "path", "p", ".", "Path to game")
	rootCmd.Flags().IntVarP(&building, "building", "b", 0, "building")
	rootCmd.Flags().IntVarP(&rotation, "rotation", "r", 0, "rotation, 0 - 3")
	rootCmd.Flags().IntVarP(&animationStep, "step", "s", 0, "animation step")
}

const (
	tileWidth  = 64 // Breite eines Tiles in Pixel
	tileHeight = 31 // Höhe eines Tiles in Pixel
)

var (
	rotationOffsets = [][][]int{
		{{0, 0}, {1, 0}, {0, 1}, {1, 1}}, // Rotation 0
		{{1, 0}, {1, 1}, {0, 0}, {0, 1}}, // Rotation 1
		{{1, 1}, {0, 1}, {1, 0}, {0, 0}}, // Rotation 2
		{{0, 1}, {0, 0}, {1, 1}, {1, 0}}, // Rotation 3
	}
)

// func (g *Game) DrawIsometricGrid(screen *ebiten.Image) {
// 	tileWidth := 64  // Breite eines Tiles in Pixel
// 	tileHeight := 32 // Höhe eines Tiles in Pixel

// 	// Mitte des Bildschirms
// 	centerX := g.windowWidth / 2
// 	centerY := g.windowHeight / 2

// 	// Vertikale Linien (von links oben nach rechts unten)
// 	for x := -g.windowHeight / tileHeight; x <= g.windowWidth/tileWidth; x++ {
// 		startX := float32(centerX + x*tileWidth/2)
// 		startY := float32(centerY + x*tileHeight/2)
// 		endX := float32(startX - float32(g.windowHeight))
// 		endY := float32(startY + float32(g.windowHeight))

// 		vector.StrokeLine(screen, startX, startY, endX, endY, 1, color.RGBA{0, 255, 0, 255}, true)
// 	}

// 	// Horizontale Linien (von links unten nach rechts oben)
// 	for y := -g.windowWidth / tileWidth; y <= g.windowHeight/tileHeight; y++ {
// 		startX := float32(centerX + y*tileWidth/2)
// 		startY := float32(centerY - y*tileHeight/2)
// 		endX := float32(startX + float32(g.windowWidth))
// 		endY := float32(startY + float32(g.windowWidth/2))

// 		vector.StrokeLine(screen, startX, startY, endX, endY, 1, color.RGBA{0, 255, 255, 255}, true)
// 	}
// }

// func PrintGrid(dst *ebiten.Image, x1, y1 float64, color color.Color, tileSize int) {
// 	x1i, y1i := rot.CartesianToIso(float64(x1), float64(y1), tileSize)
// 	x2i, y2i := rot.CartesianToIso(float64(x1+float64(tileSize)), y1+float64(tileSize), tileSize)

//		vector.StrokeLine(dst, float32(x1i), float32(y1i), float32(x2i), float32(y2i), 1, color, false)
//	}
func (g *Game) Draw(screen *ebiten.Image) {
	offsets := rotationOffsets[g.Building.Rotation]

	for i, offset := range offsets {
		screenX := float64(g.Building.X) + float64(offset[0]-offset[1])*(float64(tileWidth)/2)
		screenY := float64(g.Building.Y) + float64(offset[0]+offset[1])*(float64(tileHeight)/2)

		// Berechne den Index-Key für das aktuelle Tile
		textureKey := calculateTextureKey(g.Building.BaseIndex, g.Building.Rotation, i)

		// Hole das Bild aus dem Texture-Atlas
		tileImg, ok := g.gfxStadtfldBsh.Images[textureKey]
		if !ok {
			log.Printf("Texture key %s not found in texture atlas", textureKey)
			continue
		}

		// Berechne den Offset für die Unterkante des Tiles
		baseOffsetY := calculateBaseOffsetY(tileImg.Bounds().Dy(), tileHeight)

		// Konvertiere `image.Image` zu `ebiten.Image`
		ebitenImg := ebiten.NewImageFromImage(tileImg)

		// Zeichne das Tile auf den Bildschirm
		options := &ebiten.DrawImageOptions{}
		options.GeoM.Translate(screenX, screenY-float64(baseOffsetY))
		options.GeoM.Scale(1.5, 1.5)
		screen.DrawImage(ebitenImg, options)
	}
}

func calculateBaseOffsetY(tileHeight, gridTileHeight int) int {
	// Berechnet die Differenz zwischen der tatsächlichen Höhe des Tiles und der erwarteten Höhe
	if tileHeight > gridTileHeight {
		return tileHeight - gridTileHeight
	}
	return 0
}

// Berechnet den Key für die aktuelle Grafik im Texture-Atlas
func calculateTextureKey(baseIndex int, rotation, tileIndex int) string {
	// Berechnung: Basisindex + (Rotation * 4) + aktueller Tile-Index
	return fmt.Sprintf("%d", baseIndex+(rotation*4)+tileIndex)
}

// Update aktualisiert den Zustand des Spiels pro Frame
func (g *Game) Update() error {
	if ebiten.IsKeyPressed(ebiten.KeyLeft) {
		g.Building.Rotation = (g.Building.Rotation + 3) % 4
	} else if ebiten.IsKeyPressed(ebiten.KeyRight) {
		g.Building.Rotation = (g.Building.Rotation + 1) % 4
	} else if ebiten.IsKeyPressed(ebiten.KeyEscape) {
		os.Exit(0)
	}
	return nil
}

// Layout legt die Fenstergröße fest
func (g *Game) Layout(outsideWidth, outsideHeight int) (int, int) {
	return ScreenWidth, ScreenHeight
}
