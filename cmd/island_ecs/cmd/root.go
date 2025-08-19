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
	"os"
	"path/filepath"
	"time"

	rl "github.com/gen2brain/raylib-go/raylib"
	"github.com/siredmar/mdcii-engine/pkg/bsh"
	"github.com/siredmar/mdcii-engine/pkg/cod"
	buildingsCod "github.com/siredmar/mdcii-engine/pkg/cod/buildings"
	"github.com/siredmar/mdcii-engine/pkg/ecs/components"
	"github.com/siredmar/mdcii-engine/pkg/ecs/systems"
	"github.com/siredmar/mdcii-engine/pkg/ecs/world"
	"github.com/siredmar/mdcii-engine/pkg/files"
	"github.com/siredmar/mdcii-engine/pkg/gam"
	r3d "github.com/siredmar/mdcii-engine/pkg/renderer/raylib"
	animations "github.com/siredmar/mdcii-engine/pkg/texture/animations"
	"github.com/siredmar/mdcii-engine/pkg/texture/atlas"
	"github.com/siredmar/mdcii-engine/pkg/world/rotation"
	"github.com/siredmar/mdcii-engine/pkg/world/zoom"
	"github.com/spf13/cobra"

	donburi "github.com/yohamta/donburi"
	"github.com/yohamta/donburi/filter"
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
		atlasPath := "/tmp/atlas"
		var a *atlas.TextureAtlas
		name := "texture-atlas"
		a, err = atlas.LoadAtlasFromJSON(fmt.Sprintf("%s/%s.json", atlasPath, name))
		if err != nil {
			fmt.Println("Error loading texture atlas. Creating new one.")
			a, err = atlas.New(4096, 4096, buildings, atlas.WithName("texture-atlas"), atlas.WithImages(gfxStadtfldBsh), atlas.WithOutputDir(atlasPath), atlas.WithName(name))
			if err != nil {
				fmt.Println("Error:", err)
				return
			}
			if err := a.Export(); err != nil {
				fmt.Println("Error exporting texture atlas:", err)
				return
			}
		}
		a, err = atlas.LoadAtlasFromJSON(fmt.Sprintf("%s/%s.json", atlasPath, name))
		if err != nil {
			fmt.Println("Error:", err)
			return
		}

		rl.InitWindow(int32(ScreenWidth), int32(ScreenHeight), "animations")
		rl.SetTargetFPS(60)
		renderer := r3d.NewRenderer(float32(zoom.TileSize()))
		defer rl.CloseWindow()

		textures := make([]rl.Texture2D, len(a.Images))
		for i, img := range a.Images {
			rlImg := rl.NewImageFromImage(img)
			textures[i] = rl.LoadTextureFromImage(rlImg)
			rl.UnloadImage(rlImg)
			defer rl.UnloadTexture(textures[i])
		}

		ani, err := animations.New(a)
		if err != nil {
			fmt.Println("Error:", err)
			return
		}

		gamParser, err := gam.NewParser()
		if err != nil {
			fmt.Println(err)
			os.Exit(1)
		}
		err = gamParser.LoadPath("/home/armin/spiele/anno1602/SAVEGAME/lastgame.gam")
		// err = gamParser.LoadPath("/home/armin/spiele/anno1602/NORDNAT/LIT02.SCP")

		if err != nil {
			fmt.Println(err)
			os.Exit(1)
		}
		err = gamParser.Parse(buildings)
		if err != nil {
			fmt.Println(err)
			os.Exit(1)
		}

		w := world.New()
		// Create an entity and get its Entry
		w.World.Create(components.AnimationType, components.TileType, components.PositionType, components.BuildingType, components.IslandType)
		// island := components.CreateIsland(w.World, ani, 10, 10, 10, 10)
		islandEntry := components.CreateIslandFromChunk(w.World, ani, gamParser.Islands5[0], 10, 10)

		// Center the camera on the loaded island so tiles are visible on start
		islandComp := components.IslandType.Get(islandEntry)

		cameraEntity := w.World.Create(components.CameraType)
		cameraEntry := w.World.Entry(cameraEntity)
		components.CameraType.Set(cameraEntry, &components.Camera{
			X:        islandComp.X + float64(islandComp.Width)/2,
			Y:        islandComp.Y + float64(islandComp.Height)/2,
			Zoom:     1.0,
			Rotation: rotation.DEG0,
		})

		controlEntity := w.World.Create(components.ControlType)
		controlEntry := w.World.Entry(controlEntity)

		components.ControlType.Set(controlEntry, &components.Control{
			Rotation:         rotation.DEG0,
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
			world:         w,
			animations:    ani,
			renderer:      renderer,
			buildingIndex: buildingIndex,
			buildings:     buildings,
			rotation:      rotation.Rotation(rotationArg),
			grid:          true,
			textures:      textures,
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
		for !rl.WindowShouldClose() {
			game.Update()
			game.Draw()
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
	world         *world.World
	animations    *animations.Animations
	renderer      *r3d.Renderer
	buildingIndex int
	rotation      rotation.Rotation
	buildings     *buildingsCod.Buildings
	grid          bool
	textures      []rl.Texture2D
}

func (g *Game) Draw() {
	var rot rotation.Rotation
	var grid bool

	controlQuery := donburi.NewQuery(filter.Contains(components.ControlType))
	controlQuery.Each(g.world.World, func(entry *donburi.Entry) {
		ctrl := components.ControlType.Get(entry)
		rot = ctrl.Rotation
		grid = ctrl.GridVisible
	})

	rl.BeginDrawing()
	rl.ClearBackground(rl.Black)
	systems.RenderSystem(g.world.World, g.renderer, g.textures, grid, rot)
	g.DrawUsage()
	rl.EndDrawing()
	// systems.MouseSelectorSystem(g.world.World)
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

func (g *Game) DrawUsage() {
	rl.DrawText("Up: Animation Step, Left/Right: Rotate, N: next, M: previous", 10, int32(ScreenHeight-20), 10, rl.White)
}

func (g *Game) Update() {
	systems.AnimationSystem(g.world.World, g.animations, 1.0/60.0)
	systems.InputSystem(g.world.World)
}
