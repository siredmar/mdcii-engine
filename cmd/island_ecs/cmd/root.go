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
	"bytes"
	"fmt"
	"image"
	"image/color"
	"image/png"
	"log"
	"os"
	"path/filepath"
	"time"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/hajimehoshi/ebiten/v2/text"
	"github.com/siredmar/mdcii-engine/pkg/bsh"
	"github.com/siredmar/mdcii-engine/pkg/cod"
	buildingsCod "github.com/siredmar/mdcii-engine/pkg/cod/buildings"
	"github.com/siredmar/mdcii-engine/pkg/ecs/components"
	"github.com/siredmar/mdcii-engine/pkg/ecs/systems"
	"github.com/siredmar/mdcii-engine/pkg/ecs/world"
	"github.com/siredmar/mdcii-engine/pkg/files"
	"github.com/siredmar/mdcii-engine/pkg/gam"
	animations "github.com/siredmar/mdcii-engine/pkg/texture/animations"
	"github.com/siredmar/mdcii-engine/pkg/texture/atlas"
	"github.com/siredmar/mdcii-engine/pkg/world/rotation"
	"github.com/spf13/cobra"
	"golang.org/x/image/font/basicfont"

	donburi "github.com/yohamta/donburi"
	"github.com/yohamta/donburi/filter"
)

var (
	gamePath      string
	buildingIndex int
	rotationArg   int

	screenshotPath        string
	screenshotAfterFrames int
	exitAfterScreenshot   bool
	// buildingParam int
)

var (
	ScreenWidth  int = 1024
	ScreenHeight int = 1024
)

func init() {
	rootCmd.Flags().StringVarP(&gamePath, "path", "p", ".", "Path to game")
	rootCmd.Flags().IntVarP(&buildingIndex, "buildingIndex", "i", 381, "building index")
	rootCmd.Flags().IntVarP(&rotationArg, "rotation", "r", 0, "rotation")

	rootCmd.Flags().StringVar(&screenshotPath, "screenshot", "", "Write a screenshot PNG to this path")
	rootCmd.Flags().IntVar(&screenshotAfterFrames, "screenshotAfterFrames", 60, "Take screenshot after N update frames")
	rootCmd.Flags().BoolVar(&exitAfterScreenshot, "exitAfterScreenshot", true, "Exit after taking the screenshot")
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

		gameRoot := absPath
		if fi, err := os.Stat(absPath); err == nil && !fi.IsDir() {
			gameRoot = filepath.Dir(absPath)
		}

		files.CreateInstance(gameRoot)
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
		atlasJsonPath := filepath.Join("/tmp/atlas", "texture-atlas.json")
		var atlasObj *atlas.TextureAtlas

		if b, err := os.ReadFile(atlasJsonPath); err == nil {
			// Cache invalidation: older atlases don't have pivot metadata or correct tile drawing.
			if !bytes.Contains(b, []byte("\"pivotX\"")) || !bytes.Contains(b, []byte("\"version\": 3")) {
				_ = os.RemoveAll(filepath.Dir(atlasJsonPath))
			}
		}

		if _, err := os.Stat(atlasJsonPath); os.IsNotExist(err) {
			fmt.Println("Atlas does not exist, creating new atlas...")
			atlasObj, err = atlas.New(atlasWidth, atlasHeight, buildings, atlas.WithName("texture-atlas"), atlas.WithImages(gfxStadtfldBsh), atlas.WithOutputDir("/tmp/atlas"))
			if err != nil {
				fmt.Println("Error:", err)
				return
			}
			if err := atlasObj.Export(); err != nil {
				fmt.Println("Error exporting texture atlas:", err)
				return
			}
			fmt.Println("Atlas created and exported.")
		} else {
			fmt.Println("Loading existing atlas...")
			atlasObj, err = atlas.LoadAtlasFromJSON(atlasJsonPath)
			if err != nil {
				fmt.Println("Error loading atlas:", err)
				return
			}
			fmt.Printf("Atlas loaded: %s (%dx%d)\n", atlasObj.AtlasMeta.Name, atlasObj.AtlasMeta.Width, atlasObj.AtlasMeta.Height)
		}
		ani, err := animations.New(atlasObj)
		if err != nil {
			fmt.Println("Error:", err)
			return
		}

		gamParser, err := gam.NewParser()
		if err != nil {
			fmt.Println(err)
			os.Exit(1)
		}
		gamPath := filepath.Join(gameRoot, "SAVEGAME", "lastgame.gam")
		err = gamParser.LoadPath(gamPath)
		// err = gamParser.LoadPath(filepath.Join(gameRoot, "NORDNAT", "LIT02.SCP"))

		if err != nil {
			fmt.Println(err)
			os.Exit(1)
		}
		err = gamParser.Parse(buildings)
		if err != nil {
			fmt.Println(err)
			os.Exit(1)
		}

		ebiten.SetWindowSize(ScreenWidth, ScreenHeight)
		ebiten.SetWindowTitle("animations")

		w := world.New()
		// Create an entity and get its Entry
		w.World.Create(components.AnimationType, components.TileType, components.PositionType, components.BuildingType, components.IslandType)
		// island := components.CreateIsland(w.World, ani, 10, 10, 10, 10)
		components.CreateIslandFromChunk(w.World, ani, gamParser.Islands5[0], 10, 10)

		cameraEntity := w.World.Create(components.CameraType)
		cameraEntry := w.World.Entry(cameraEntity)
		// Center camera to match reference view
		components.CameraType.Set(cameraEntry, &components.Camera{
			X:        60,
			Y:        -130,
			Zoom:     1.0,
			Rotation: rotation.DEG0,
		})

		controlEntity := w.World.Create(components.ControlType)
		controlEntry := w.World.Entry(controlEntity)

		components.ControlType.Set(controlEntry, &components.Control{
			Rotation:         rotation.Rotation(rotationArg),
			GridVisible:      true,
			LastKeyPressTime: time.Now(),
		})

		// fmt.Println(island)
		// w.World.Entry(entity)

		// fmt.Println(w.World)

		// buildingId, err := buildings.GetBuildingIdByIndex(buildingIndex)
		// if err != nil {
		// 	log.Fatalln("Error:", err)
		// }

		game := &Game{
			world:      w,
			animations: ani,
			// animation:    nil,
			buildingIndex: buildingIndex,
			// buildingId: id,
			// count:     0,
			buildings: buildings,
			rotation:  rotation.Rotation(rotationArg),
			// entry:     entry,
			grid: true,

			screenshotPath:        screenshotPath,
			screenshotAfterFrames: screenshotAfterFrames,
			exitAfterScreenshot:   exitAfterScreenshot,
		}

		// components.BuildingType.Set(entry, &components.Building{
		// 	BuildingID: buildingId,
		// 	Rotation:   game.rotation,
		// })

		// // Set initial values for the Animation component
		// components.AnimationType.Set(entry, &components.Animation{
		// 	Running:  true,
		// 	Duration: 1,
		// 	Loop:     true,
		// })

		// components.TileType.Set(entry, &components.Tile{
		// 	Image: nil,
		// })

		// components.PositionType.Set(entry, &components.Position{
		// 	X: 100,
		// 	Y: 100,
		// })

		// game.entry = entry

		// game.buildingId, err = buildings.GetBuildingIdByIndex(game.buildingIndex)
		// if err != nil {
		// 	fmt.Println("Error:", err)
		// 	return
		// }
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
	world        *world.World
	animations   *animations.Animations
	ScreenWidth  int
	ScreenHeight int
	// lastKeyPressTime time.Time
	// buildingId       int
	buildingIndex int
	rotation      rotation.Rotation
	// lastTime         time.Time
	// entry            *donburi.Entry
	buildings *buildingsCod.Buildings
	grid      bool

	screenshotPath        string
	screenshotAfterFrames int
	exitAfterScreenshot   bool
	frameCount            int
	screenshotPending     bool
	screenshotTaken       bool
}

func (g *Game) Draw(screen *ebiten.Image) {
	// Clear screen with black background
	screen.Fill(color.Black)

	var rot rotation.Rotation
	var grid bool

	controlQuery := donburi.NewQuery(filter.Contains(components.ControlType))
	controlQuery.Each(g.world.World, func(entry *donburi.Entry) {
		ctrl := components.ControlType.Get(entry)
		rot = ctrl.Rotation
		grid = ctrl.GridVisible
	})
	systems.RenderSystem(g.world.World, screen, grid, rot)

	if g.screenshotPending && !g.screenshotTaken {
		if err := g.writeScreenshot(screen); err != nil {
			log.Println("screenshot error:", err)
		} else {
			log.Println("wrote screenshot:", g.screenshotPath)
		}
		g.screenshotTaken = true
		g.screenshotPending = false
	}
	// systems.MouseSelectorSystem(g.world.World) // Add the mouse selector system
	// systems.RenderSystemAscii(g.world.World)
}

// func (g *Game) DrawBuildingInfo(screen *ebiten.Image) {
// 	textColor := color.RGBA{255, 255, 255, 255}
// 	face := basicfont.Face7x13

// 	b := g.buildings.BuildingsVector[g.buildingId]
// 	text.Draw(screen, fmt.Sprintf("Building Id: %d", b.Id), face, 10, 10, textColor)
// 	text.Draw(screen, fmt.Sprintf("Building Size: %d,%d", b.Size.W, b.Size.H), face, 10, 20, textColor)
// 	text.Draw(screen, fmt.Sprintf("Building Animation Amount: %d", b.AnimationAmount), face, 10, 30, textColor)
// 	text.Draw(screen, fmt.Sprintf("Building Animation Add: %d", b.AnimationAdd), face, 10, 40, textColor)
// 	text.Draw(screen, fmt.Sprintf("Building Gfx: %d", b.Gfx), face, 10, 50, textColor)
// 	// text.Draw(screen, fmt.Sprintf("Building Rotation: %d", g.Building.Rotation), face, 10, 60, textColor)
// 	// text.Draw(screen, fmt.Sprintf("Building BaseIndex: %d", g.Building.BaseIndex), face, 10, 70, textColor)
// 	// text.Draw(screen, fmt.Sprintf("CurrentAnimationStep: %d", g.Building.CurrentAnimationStep), face, 10, 80, textColor)
// }

func (g *Game) DrawUsage(screen *ebiten.Image) {
	textColor := color.RGBA{255, 255, 255, 255}
	face := basicfont.Face7x13

	text.Draw(screen, "Up: Animation Step, Left/Right: Rotate, N: next, M: previous", face, 10, ScreenHeight-20, textColor)

}

func (g *Game) Update() error {
	g.frameCount++
	if g.exitAfterScreenshot && g.screenshotTaken {
		return ebiten.Termination
	}

	systems.AnimationSystem(g.world.World, g.animations, 1.0/60.0)
	systems.InputSystem(g.world.World)

	if g.screenshotPath != "" && !g.screenshotTaken && !g.screenshotPending && g.frameCount >= g.screenshotAfterFrames {
		g.screenshotPending = true
	}
	return nil
	// const debounceDuration = time.Millisecond * 250
	// now := time.Now()

	// if now.Sub(g.lastKeyPressTime) >= debounceDuration {
	// 	if ebiten.IsKeyPressed(ebiten.KeyLeft) {
	// 		fmt.Println(int(g.rotation))
	// 		g.rotation.Increment()
	// 		fmt.Println(int(g.rotation))
	// 		g.lastKeyPressTime = now

	// 	} else if ebiten.IsKeyPressed(ebiten.KeyRight) {
	// 		fmt.Println(int(g.rotation))
	// 		g.rotation.Decrement()
	// 		fmt.Println(int(g.rotation))
	// 		g.lastKeyPressTime = now
	// 	} else if ebiten.IsKeyPressed(ebiten.KeyG) {
	// 		g.grid = !g.grid
	// 		g.lastKeyPressTime = now
	// 	} else if ebiten.IsKeyPressed(ebiten.KeyEscape) {
	// 		os.Exit(0)
	// 	}
	// }
	// g.lastTime = now
	// return nil
}

func (g *Game) Layout(outsideWidth, outsideHeight int) (int, int) {
	return ScreenWidth, ScreenHeight
}

func (g *Game) writeScreenshot(screen *ebiten.Image) error {
	if err := os.MkdirAll(filepath.Dir(g.screenshotPath), 0o755); err != nil {
		return err
	}

	w, h := screen.Bounds().Dx(), screen.Bounds().Dy()
	pixels := make([]byte, 4*w*h)
	screen.ReadPixels(pixels)

	img := image.NewRGBA(image.Rect(0, 0, w, h))
	copy(img.Pix, pixels)

	f, err := os.Create(g.screenshotPath)
	if err != nil {
		return err
	}
	defer f.Close()

	return png.Encode(f, img)
}
