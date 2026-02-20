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
	"image/color"
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
	animations "github.com/siredmar/mdcii-engine/pkg/texture/animations"
	"github.com/siredmar/mdcii-engine/pkg/texture/atlas"
	"github.com/siredmar/mdcii-engine/pkg/world/rotation"
	"github.com/spf13/cobra"
	"golang.org/x/image/font/basicfont"

	donburi "github.com/yohamta/donburi"
)

var (
	gamePath      string
	buildingIndex int
	rotationArg   int
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

		ebiten.SetWindowSize(ScreenWidth, ScreenHeight)
		ebiten.SetWindowTitle("animations")

		w := world.New()

		// Create camera and control entities required by RenderSystem
		camEntity := w.World.Create(components.CameraType)
		camEntry := w.World.Entry(camEntity)
		components.CameraType.Set(camEntry, &components.Camera{Zoom: 1.0})

		ctrlEntity := w.World.Create(components.ControlType)
		ctrlEntry := w.World.Entry(ctrlEntity)
		components.ControlType.Set(ctrlEntry, &components.Control{HoveredIsland: -1, HoveredTileID: -1})

		// Create an entity and get its Entry
		entity := w.World.Create(components.AnimationType, components.TileType, components.PositionType, components.BuildingType)
		entry := w.World.Entry(entity)

		buildingId, err := buildings.GetBuildingIdByIndex(buildingIndex)
		if err != nil {
			log.Fatalln("Error:", err)
		}

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
		}

		components.BuildingType.Set(entry, &components.Building{
			BuildingID: buildingId,
			Rotation:   game.rotation,
		})

		// Set initial values for the Animation component
		components.AnimationType.Set(entry, &components.Animation{
			Running:  true,
			Duration: 1,
			Loop:     true,
		})

		components.TileType.Set(entry, &components.Tile{
			Image: nil,
		})

		components.PositionType.Set(entry, &components.Position{
			X: 100,
			Y: 100,
		})

		game.entry = entry

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
	world            *world.World
	animations       *animations.Animations
	ScreenWidth      int
	ScreenHeight     int
	lastKeyPressTime time.Time
	buildingId       int
	buildingIndex    int
	rotation         rotation.Rotation
	lastTime         time.Time
	entry            *donburi.Entry
	buildings        *buildingsCod.Buildings
}

func (g *Game) Draw(screen *ebiten.Image) {
	tile := components.TileType.Get(g.entry)
	if tile.Image != nil {
		op := &ebiten.DrawImageOptions{}
		// Center the sprite on screen
		imgW := float64(tile.Image.Bounds().Dx())
		imgH := float64(tile.Image.Bounds().Dy())
		op.GeoM.Translate(float64(ScreenWidth)/2-imgW/2, float64(ScreenHeight)/2-imgH/2)
		screen.DrawImage(tile.Image, op)
	}

	bld := components.BuildingType.Get(g.entry)
	face := basicfont.Face7x13
	textColor := color.RGBA{255, 255, 255, 255}
	y := 15
	text.Draw(screen, fmt.Sprintf("Building ID: %d  Index: %d", bld.BuildingID, g.buildingIndex), face, 10, y, textColor)
	y += 15
	text.Draw(screen, fmt.Sprintf("Rotation: %s", g.rotation.String()), face, 10, y, textColor)
	y += 15
	anim := components.AnimationType.Get(g.entry)
	if b := g.buildings.Buildings[bld.BuildingID]; b != nil {
		text.Draw(screen, fmt.Sprintf("Size: %dx%d  Gfx: %d  Anim: %d", b.Size.W, b.Size.H, b.Gfx, b.AnimationAmount), face, 10, y, textColor)
		y += 15
	}
	status := "Playing"
	if !anim.Running {
		status = "Paused"
	}
	text.Draw(screen, fmt.Sprintf("Frame: %d/%d  [%s]", anim.CurrentFrame, anim.Count, status), face, 10, y, textColor)

	g.DrawUsage(screen)
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

	text.Draw(screen, "Up/Down: Step Frame, Space: Play/Pause, Left/Right: Rotate, N/M: Next/Prev Building", face, 10, ScreenHeight-20, textColor)

}

func (g *Game) Update() error {
	systems.AnimationSystem(g.world.World, g.animations, 1.0/60.0)

	const debounceDuration = time.Millisecond * 250
	now := time.Now()

	if now.Sub(g.lastKeyPressTime) >= debounceDuration {
		if ebiten.IsKeyPressed(ebiten.KeyLeft) {
			fmt.Println(int(g.rotation))
			g.rotation.Increment()
			components.BuildingType.Set(g.entry, &components.Building{
				BuildingID: g.buildingId,
				Rotation:   g.rotation,
			})
			fmt.Println(int(g.rotation))
			g.lastKeyPressTime = now

		} else if ebiten.IsKeyPressed(ebiten.KeyRight) {
			fmt.Println(int(g.rotation))
			g.rotation.Decrement()
			fmt.Println(int(g.rotation))
			components.BuildingType.Set(g.entry, &components.Building{
				BuildingID: g.buildingId,
				Rotation:   g.rotation,
			})
			g.lastKeyPressTime = now
		} else if ebiten.IsKeyPressed(ebiten.KeyN) {
			fmt.Println(g.buildingId)
			g.buildingIndex = (g.buildingIndex + 1) % len(g.buildings.BuildingsVector)
			var err error
			g.buildingId, err = g.buildings.GetBuildingIdByIndex(g.buildingIndex)
			if err != nil {
				log.Fatalln("Error:", err)
			}
			components.BuildingType.Set(g.entry, &components.Building{
				BuildingID: g.buildingId,
				Rotation:   g.rotation,
			})
			fmt.Println(g.buildingId)
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
				log.Fatalln("Error:", err)
			}
			components.BuildingType.Set(g.entry, &components.Building{
				BuildingID: g.buildingId,
				Rotation:   g.rotation,
			})
			g.lastKeyPressTime = now
		} else if ebiten.IsKeyPressed(ebiten.KeyUp) {
			anim := components.AnimationType.Get(g.entry)
			anim.Running = false
			if anim.Count > 1 {
				anim.CurrentFrame = (anim.CurrentFrame + 1) % anim.Count
			}
			g.lastKeyPressTime = now
		} else if ebiten.IsKeyPressed(ebiten.KeyDown) {
			anim := components.AnimationType.Get(g.entry)
			anim.Running = false
			if anim.Count > 1 {
				anim.CurrentFrame = (anim.CurrentFrame - 1 + anim.Count) % anim.Count
			}
			g.lastKeyPressTime = now
		} else if ebiten.IsKeyPressed(ebiten.KeySpace) {
			anim := components.AnimationType.Get(g.entry)
			anim.Running = !anim.Running
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
