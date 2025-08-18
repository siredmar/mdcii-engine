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
	"os"
	"path/filepath"
	"time"

	rl "github.com/gen2brain/raylib-go/raylib"
	r3d "github.com/siredmar/mdcii-engine/pkg/renderer/raylib"

	"github.com/siredmar/mdcii-engine/pkg/bsh"
	"github.com/siredmar/mdcii-engine/pkg/building"
	"github.com/siredmar/mdcii-engine/pkg/cod"
	"github.com/siredmar/mdcii-engine/pkg/cod/buildings"
	buildingsCod "github.com/siredmar/mdcii-engine/pkg/cod/buildings"
	"github.com/siredmar/mdcii-engine/pkg/files"
	atlas "github.com/siredmar/mdcii-engine/pkg/texture/atlas"
	"github.com/siredmar/mdcii-engine/pkg/world/rotation"
	"github.com/spf13/cobra"
)

var (
	gamePath      string
	buildingIndex int
	atlasPath     string
)

var (
	ScreenWidth  int = 600
	ScreenHeight int = 600
	TileSize     int = 64
)

func init() {
	rootCmd.Flags().StringVarP(&atlasPath, "atlas", "a", ".", "Path to texture atlas")
	rootCmd.Flags().StringVarP(&gamePath, "path", "p", ".", "Path to game")
	rootCmd.Flags().IntVarP(&buildingIndex, "buildingIndex", "i", 381, "building index")
}

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
		if err = haeuserCod.Parse(); err != nil {
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

		rl.InitWindow(int32(ScreenWidth), int32(ScreenHeight), "building_from_atlas")
		defer rl.CloseWindow()
		rl.SetTargetFPS(60)

		// Load atlas images into raylib textures
		textures := make([]rl.Texture2D, len(a.Images))
		for i, img := range a.Images {
			rlImg := rl.NewImageFromImage(img)
			textures[i] = rl.LoadTextureFromImage(rlImg)
			rl.UnloadImage(rlImg)
			defer rl.UnloadTexture(textures[i])
		}

		renderer := r3d.NewRenderer(float32(TileSize))

		game := &Game{
			windowWidth:    ScreenWidth,
			windowHeight:   ScreenHeight,
			tileSize:       TileSize,
			gfxStadtfldBsh: gfxStadtfldBsh,
			buildings:      buildings,
			Building: &building.Building{
				AnimationSteps:       0,
				CurrentAnimationStep: 0,
			},
			buildingIndex:  buildingIndex,
			atlas:          a,
			rotation:       rotation.DEG0,
			animationIndex: 0,
			renderer:       renderer,
			textures:       textures,
		}

		game.setBuilding(game.buildingIndex, 0)

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

func (g *Game) setBuilding(index int, animationStep int) {
	fmt.Println("Building ID", index)
	fmt.Println("Rotation", g.rotation)
	fmt.Println("Animation Index", animationStep)
	b := g.buildings.BuildingsVector[index]
	spriteMeta := g.atlas.ImagesMeta[b.Id].Animations[g.rotation].Images[animationStep].Metadata

	g.Building.Sprite = g.atlas.ImagesMeta[b.Id].Animations[g.rotation].Images[animationStep].Sprite
	g.Building.AnimationSteps = b.AnimationAmount
	g.Building.CurrentAnimationStep = animationStep
	g.Building.Size = building.BuildingSize(b.Size.W, b.Size.H)

	// Set texture and source rectangle for renderer
	g.renderer.SetAtlasTexture(g.textures[spriteMeta.PNGIndex])
	g.src = rl.NewRectangle(float32(spriteMeta.X), float32(spriteMeta.Y), float32(spriteMeta.Width), float32(spriteMeta.Height))
}

type Game struct {
	windowWidth    int
	windowHeight   int
	tileSize       int
	gfxStadtfldBsh *bsh.BshPng
	buildings      *buildings.Buildings
	Building       *building.Building
	buildingIndex  int
	atlas          *atlas.TextureAtlas
	rotation       rotation.Rotation
	animationIndex int
	renderer       *r3d.Renderer
	textures       []rl.Texture2D
	src            rl.Rectangle
	Sprite         image.Image
}

func (g *Game) Draw() {
	rl.BeginDrawing()
	rl.ClearBackground(rl.Black)

	g.renderer.Begin()
	// g.renderer.DrawGroundTile(0, 0)
	g.renderer.DrawBillboardFromAtlas(g.src, g.src.Width, g.src.Height, 0, 0)
	g.renderer.End()

	g.DrawBuildingInfo()
	g.DrawUsage()

	rl.EndDrawing()
}

func (g *Game) DrawBuildingInfo() {
	b := g.buildings.BuildingsVector[g.buildingIndex]
	rl.DrawText(fmt.Sprintf("Building Id: %d", b.Id), 10, 10, 10, rl.RayWhite)
	rl.DrawText(fmt.Sprintf("Building Size: %d,%d", b.Size.W, b.Size.H), 10, 20, 10, rl.RayWhite)
	rl.DrawText(fmt.Sprintf("Building Animation Amount: %d", b.AnimationAmount), 10, 30, 10, rl.RayWhite)
	rl.DrawText(fmt.Sprintf("Building Animation Add: %d", b.AnimationAdd), 10, 40, 10, rl.RayWhite)
	rl.DrawText(fmt.Sprintf("Building Gfx: %d", b.Gfx), 10, 50, 10, rl.RayWhite)
	rl.DrawText(fmt.Sprintf("Building Rotation: %d", g.Building.Rotation), 10, 60, 10, rl.RayWhite)
	rl.DrawText(fmt.Sprintf("Building BaseIndex: %d", g.Building.BaseIndex), 10, 70, 10, rl.RayWhite)
	rl.DrawText(fmt.Sprintf("CurrentAnimationStep: %d", g.Building.CurrentAnimationStep), 10, 80, 10, rl.RayWhite)
}

func (g *Game) DrawUsage() {
	rl.DrawText("Up: Animation Step, Left/Right: Rotate, N: next, M: previous", 10, int32(ScreenHeight-20), 10, rl.RayWhite)
}

func (g *Game) Update() {
	time.Sleep(100 * time.Millisecond)
	if rl.IsKeyDown(rl.KeyLeft) {
		g.rotation.Decrement()
		g.setBuilding(g.buildingIndex, g.animationIndex)
	} else if rl.IsKeyDown(rl.KeyRight) {
		g.rotation.Increment()
		g.setBuilding(g.buildingIndex, g.animationIndex)
	} else if rl.IsKeyDown(rl.KeyUp) {
		if g.Building.AnimationSteps > 0 {
			g.Building.CurrentAnimationStep = (g.Building.CurrentAnimationStep + 1) % g.Building.AnimationSteps
			g.setBuilding(g.buildingIndex, g.Building.CurrentAnimationStep)
		}
	} else if rl.IsKeyDown(rl.KeyN) {
		g.buildingIndex = (g.buildingIndex + 1) % len(g.buildings.BuildingsVector)
		g.setBuilding(g.buildingIndex, g.animationIndex)
	} else if rl.IsKeyDown(rl.KeyM) {
		if g.buildingIndex > 0 {
			g.buildingIndex--
		} else {
			g.buildingIndex = len(g.buildings.BuildingsVector) - 1
		}
		g.setBuilding(g.buildingIndex, g.animationIndex)
	} else if rl.IsKeyDown(rl.KeyEscape) {
		os.Exit(0)
	}
}
