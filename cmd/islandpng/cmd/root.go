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
	"math"
	"os"
	"path/filepath"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/hajimehoshi/ebiten/v2/ebitenutil"
	"github.com/siredmar/mdcii-engine/pkg/bsh"
	"github.com/siredmar/mdcii-engine/pkg/chunks"
	"github.com/siredmar/mdcii-engine/pkg/cod"
	"github.com/siredmar/mdcii-engine/pkg/cod/buildings"
	"github.com/siredmar/mdcii-engine/pkg/files"
	"github.com/siredmar/mdcii-engine/pkg/gam"
	"github.com/siredmar/mdcii-engine/pkg/texture/atlas"
	"github.com/siredmar/mdcii-engine/pkg/texture/sprites"
	"github.com/siredmar/mdcii-engine/pkg/world/camera"
	island "github.com/siredmar/mdcii-engine/pkg/world/island/v1alpha1"
	"github.com/siredmar/mdcii-engine/pkg/world/rotation"
	"github.com/spf13/cobra"

	"github.com/spf13/viper"
	// cod "github.com/siredmar/mdcii-engine/pkg/cod"
)

var cfgFile string

var (
	gamePath string
	gamFile  string
)

var (
	ScreenWidth  int  = 2280
	ScreenHeight int  = 1320
	TileSize     int  = 64
	GridEnable   bool = false
)

// rootCmd represents the base command when called without any subcommands
var rootCmd = &cobra.Command{
	Use:   "islandpng",
	Short: "generate ",
	// Uncomment the following line if your bare application
	// has an action associated with it:
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

		buildings, err := buildings.NewBuildings(haeuserCod)
		if err != nil {
			fmt.Println(err)
			os.Exit(1)
		}
		// jsonBytes, err := json.MarshalIndent(buildings.GetBuildings(), "", "    ")
		// if err != nil {
		// 	fmt.Println(err)
		// 	os.Exit(1)
		// }

		gamParser, err := gam.NewParser()
		if err != nil {
			fmt.Println(err)
			os.Exit(1)
		}
		err = gamParser.LoadPath(gamFile)
		if err != nil {
			fmt.Println(err)
			os.Exit(1)
		}
		err = gamParser.Parse(buildings)
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

		// // convert to json gamParser.Islands5[0
		// jsonBytes, err := json.MarshalIndent(gamParser.Islands5[0], "", "    ")
		// if err != nil {
		// 	fmt.Println(err)
		// 	os.Exit(1)
		// }
		// fmt.Println(string(jsonBytes))
		gridAtlas, err := atlas.CreateTextureAtlas(100, 100, atlas.WithName("grid"), atlas.WithFiles([]string{"./assets/gfx/0.png", "./assets/gfx/1.png"}))
		if err != nil {
			fmt.Println(err)
			os.Exit(1)
		}

		gfxAtlas, err := atlas.CreateTextureAtlas(4096, 4096, atlas.WithName("gfx-stadtfld"), atlas.WithImages(gfxStadtfldBsh.Images))
		if err != nil {
			fmt.Println(err)
			os.Exit(1)
		}

		gridSprites, err := sprites.NewSprites(gridAtlas)
		if err != nil {
			fmt.Println(err)
			os.Exit(1)
		}

		gfxSprites, err := sprites.NewSprites(gfxAtlas)
		if err != nil {
			fmt.Println(err)
			os.Exit(1)
		}
		// fmt.Println(gfxSprites)

		ebiten.SetWindowSize(ScreenWidth, ScreenHeight)
		ebiten.SetWindowTitle("islandpng")
		game := &Game{
			windowWidth:    ScreenWidth,
			windowHeight:   ScreenHeight,
			tileSize:       TileSize,
			gfxSprites:     gfxSprites,
			gridSprites:    gridSprites,
			gam:            gamParser,
			buildings:      buildings,
			op:             &ebiten.DrawImageOptions{},
			cameraRotation: rotation.DEG0,
			buffer:         ebiten.NewImage(ScreenWidth, ScreenHeight),
			Camera:         camera.NewCamera(-float64(ScreenWidth/2), -float64(ScreenHeight/2), 500, 1, 1.2),
			drawToBuffer:   true,
			tileInfoX:      0,
			tileInfoY:      0,
			gKeyDebounce:   0,
			islandToLoad:   gamParser.Islands5[0],
			island:         nil,
		}
		game.island, err = island.NewIsland(island.WithChunk(game.gfxSprites, game.buildings, gamParser.Islands5[0]))
		if err != nil {
			fmt.Println(err)
			os.Exit(1)
		}

		if err := ebiten.RunGame(game); err != nil {
			log.Fatal(err)
		}
	},
}

type Game struct {
	windowWidth    int
	windowHeight   int
	tileSize       int
	gfxSprites     *sprites.Sprites
	gridSprites    *sprites.Sprites
	gam            *gam.GamParser
	buildings      *buildings.Buildings
	cameraRotation rotation.Rotation
	ScreenWidth    int
	ScreenHeight   int
	Camera         *camera.Camera
	buffer         *ebiten.Image
	op             *ebiten.DrawImageOptions
	drawToBuffer   bool
	tileInfoX      int
	tileInfoY      int
	gKeyDebounce   int
	islandToLoad   *chunks.Island5
	island         *island.Island
}

func (g *Game) Update() error {
	dt := 1.0 / 60
	if ebiten.IsKeyPressed(ebiten.KeyLeft) || ebiten.IsKeyPressed(ebiten.KeyA) {
		g.Camera.X -= g.Camera.Speed * dt / g.Camera.Zoom
		g.drawToBuffer = true
	}
	if ebiten.IsKeyPressed(ebiten.KeyRight) || ebiten.IsKeyPressed(ebiten.KeyD) {
		g.Camera.X += g.Camera.Speed * dt / g.Camera.Zoom
		g.drawToBuffer = true
	}
	if ebiten.IsKeyPressed(ebiten.KeyDown) || ebiten.IsKeyPressed(ebiten.KeyS) {
		g.Camera.Y -= g.Camera.Speed * dt / g.Camera.Zoom
		g.drawToBuffer = true
	}
	if ebiten.IsKeyPressed(ebiten.KeyUp) || ebiten.IsKeyPressed(ebiten.KeyW) {
		g.Camera.Y += g.Camera.Speed * dt / g.Camera.Zoom
		g.drawToBuffer = true
	}
	if ebiten.IsKeyPressed(ebiten.KeyG) {
		g.gKeyDebounce++
		if g.gKeyDebounce > 4 {
			GridEnable = !GridEnable
			g.gKeyDebounce = 0
		}
	}
	if ebiten.IsKeyPressed(ebiten.KeyEscape) {
		os.Exit(0)
	}
	_, sY := ebiten.Wheel()
	g.Camera.Zoom *= math.Pow(g.Camera.ZoomSpeed, sY)

	if sY != 0 {
		g.drawToBuffer = true
	}

	// Get the cursor position
	mx, my := ebiten.CursorPosition()
	// Offset for center
	fmx := float64(mx) - float64(g.windowWidth)/2.0
	fmy := float64(my) - float64(g.windowHeight)/2.0
	// x, y := float64(mx)+float64(g.windowWidth/2.0), float64(my)+float64(g.windowHeight/2.0)
	// Translate it to game coordinates
	x, y := (float64(fmx/g.Camera.Zoom) + g.Camera.X), float64(fmy/g.Camera.Zoom)-g.Camera.Y

	// Do a half tile mouse shift because of our perspective
	x -= .5 * float64(g.tileSize)
	y -= .5 * float64(g.tileSize)
	// Convert isometric
	imx, imy := rotation.IsoToCartesian(x, y, g.tileSize)
	imx = math.Floor(imx) + 1
	imy = math.Floor(imy) + 1
	g.tileInfoX = int(imx)
	g.tileInfoY = int(imy)
	return nil
}

// var once sync.Once
// update is called every frame (1/60 [s]).
func (g *Game) Draw(screen *ebiten.Image) {
	// fmt.Println("Rendering frame", g.frames)

	// Write your game's logical update.

	// // Draw only if we have to (and only draw the visible ones)
	g.buffer.Clear()
	g.render(g.buffer)
	g.drawToBuffer = false
	screen.DrawImage(g.buffer, nil)
	ebitenutil.DebugPrint(screen, fmt.Sprintf("TPS %f, FPS %f", ebiten.ActualTPS(), ebiten.ActualFPS()))
	ebitenutil.DebugPrintAt(screen, fmt.Sprintf("Camera: X: %f, Y: %f, Zoom: %f", g.Camera.X, g.Camera.Y, g.Camera.Zoom), 0, 20)

	// g.lastMousePosX = mx
	// g.lastMousePosY = my

}

// func PrintGrid(dst *ebiten.Image, x1, y1 float64, color color.Color, tileSize int) {
// 	x1i, y1i := rotation.CartesianToIso(float64(x1), float64(y1), tileSize)
// 	x2i, y2i := rotation.CartesianToIso(float64(x1+float64(tileSize)), y1+float64(tileSize), tileSize)

// 	vector.StrokeLine(dst, float32(x1i), float32(y1i), float32(x2i), float32(y2i), 1, color, false)
// }

func (g *Game) render(screen *ebiten.Image) {
	g.island.Render(rotation.DEG0, screen)
	// for y := range g.gam.Islands5[0].Height {
	// 	for x := range g.gam.Islands5[0].Width {
	// 		t := g.gam.Islands5[0].Layers.Top.Fields[y*g.gam.Islands5[0].Width+x]
	// 		if t.Id == 65535 || t.Id == 102 {
	// 			continue
	// 		}
	// 		if t.Id == 1201 {
	// 			fmt.Println("found 1201")
	// 		}
	// 		building := g.buildings.Buildings[t.Id]
	// 		tile := tiles.NewTerrainTile(rotation.Rotation(t.Orientation), t.Posx, t.Posy, g.buildings.Buildings[t.Id], g.gfxSprites)
	// 		if g.tileInfoX == x && g.tileInfoY == y {
	// 			ebitenutil.DebugPrintAt(screen, fmt.Sprintf("Tile: %d, X: %d, Y: %d, Orientation: %d, GFX: %d, PosOffset: %d", t.Id, x, y, t.Orientation, tile.Gfx[tile.Rotation], building.PositionOffset), 0, 40)
	// 			ebitenutil.DebugPrintAt(screen, fmt.Sprintf("Type: %s", building.Kind.String()), 0, 60)
	// 		}

	// 		xi, yi := rotation.CartesianToIso(float64(x), float64(y), g.tileSize)
	// 		g.op.GeoM.Reset()
	// 		//Translate for isometric
	// 		g.op.GeoM.Translate(float64(xi), float64(yi))
	// 		// Translate for tile offset
	// 		g.op.GeoM.Translate(0, -float64(tile.CalcOffset()))
	// 		//Scale for camera zoom
	// 		g.op.GeoM.Scale(g.Camera.Zoom, g.Camera.Zoom)
	// 		//Translate for center of screen offset
	// 		g.op.GeoM.Translate(float64(g.windowWidth/2.0), float64(g.windowHeight/2.0))
	// 		//Translate for camera position
	// 		g.op.GeoM.Translate(-g.Camera.X, g.Camera.Y)
	// 		// gridOp := &ebiten.DrawImageOptions{}
	// 		// gridOp.GeoM.Translate(float64(xi), float64(yi))
	// 		// gridOp.GeoM.Scale(g.Camera.Zoom, g.Camera.Zoom)
	// 		// gridOp.GeoM.Translate(float64(g.windowWidth/2.0), float64(g.windowHeight/2.0))
	// 		// gridOp.GeoM.Translate(-g.Camera.X, g.Camera.Y)

	// 		img := g.gfxSprites.Sprites[tile.Gfx[tile.Rotation]].Image
	// 		screen.DrawImage(img, g.op)
	// 		// if GridEnable {
	// 		// 	screen.DrawImage(g.gridSprites.Sprites[0].Image, gridOp)
	// 		// }

	// 	}
	// }
}

func (g *Game) Layout(outsideWidth, outsideHeight int) (screenWidth, screenHeight int) {
	return ScreenWidth, ScreenHeight
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
	cobra.OnInitialize(initConfig)

	// Cobra also supports local flags, which will only run
	// when this action is called directly.
	// rootCmd.Flags().BoolVarP(&decrypt, "decrypt", "d", false, "decrypt true/false")
	rootCmd.Flags().StringVarP(&gamePath, "path", "p", ".", "Path to game")

	rootCmd.Flags().StringVarP(&gamFile, "gam", "g", "", "gam file path")
}

// initConfig reads in config file and ENV variables if set.
func initConfig() {
	if cfgFile != "" {
		// Use config file from the flag.
		viper.SetConfigFile(cfgFile)
	}

	viper.AutomaticEnv() // read in environment variables that match

	// If a config file is found, read it in.
	if err := viper.ReadInConfig(); err == nil {
		fmt.Println("Using config file:", viper.ConfigFileUsed())
	}
}
