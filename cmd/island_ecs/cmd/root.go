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

	rl "github.com/gen2brain/raylib-go/raylib"
	"github.com/siredmar/mdcii-engine/pkg/bsh"
	"github.com/siredmar/mdcii-engine/pkg/cod"
	buildingsCod "github.com/siredmar/mdcii-engine/pkg/cod/buildings"
	"github.com/siredmar/mdcii-engine/pkg/ecs/components"
	"github.com/siredmar/mdcii-engine/pkg/files"
	"github.com/siredmar/mdcii-engine/pkg/gam"
	"github.com/siredmar/mdcii-engine/pkg/texture/animations"
	"github.com/siredmar/mdcii-engine/pkg/texture/atlas"
	"github.com/siredmar/mdcii-engine/pkg/world/zoom"
	"github.com/spf13/cobra"
)

var (
	gamePath      string
	buildingIndex int
	rotationArg   int
	newatlas      bool
	// buildingParam int
)

var (
	ScreenWidth  int = 1024
	ScreenHeight int = 1024
)

func init() {
	rootCmd.Flags().StringVarP(&gamePath, "path", "p", ".", "Path to game")
	rootCmd.Flags().BoolVarP(&newatlas, "newatlas", "a", false, "Create a new atlas")
}

// Game (V2) ---------------------------------------------------------------
// Old ECS fields removed; lean V2 fields only.
type Game struct {
	textures       []rl.Texture2D
	entitiesV2     []EntityV2
	islandV2       components.IslandV2
	cameraV2       components.Camera
	globalRotDirty bool
}

var rootCmd = &cobra.Command{
	Use:   "island_ecs",
	Short: "island_ecs",
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
		atlasPath := "./atlas"
		var a *atlas.TextureAtlas
		name := "texture-atlas"
		if !newatlas {
			a, err = atlas.LoadAtlasFromJSON(fmt.Sprintf("%s/%s.json", atlasPath, name))
			if err != nil {
				fmt.Println("Error loading texture atlas. Creating new one.")
				a, err = atlas.New(2096, 2096, buildings, atlas.WithName("texture-atlas"), atlas.WithImages(gfxStadtfldBsh), atlas.WithOutputDir(atlasPath), atlas.WithName(name))
				if err != nil {
					fmt.Println("Error:", err)
					return
				}
				if err := a.Export(); err != nil {
					fmt.Println("Error exporting texture atlas:", err)
					return
				}
				a, err = atlas.LoadAtlasFromJSON(fmt.Sprintf("%s/%s.json", atlasPath, name))
				if err != nil {
					fmt.Println("Error:", err)
					return
				}
			}
		} else {
			a, err = atlas.New(2096, 2096, buildings, atlas.WithName("texture-atlas"), atlas.WithImages(gfxStadtfldBsh), atlas.WithOutputDir(atlasPath), atlas.WithName(name))
			if err != nil {
				fmt.Println("Error:", err)
				return
			}
			if err := a.Export(); err != nil {
				fmt.Println("Error exporting texture atlas:", err)
				return
			}
		}
		zoom.ZoomIn()
		rl.InitWindow(int32(ScreenWidth), int32(ScreenHeight), "ecs_v2")
		rl.SetTargetFPS(60)
		defer rl.CloseWindow()

		textures := make([]rl.Texture2D, len(a.Images))
		for i, img := range a.Images {
			rlImg := rl.NewImageFromImage(img)
			textures[i] = rl.LoadTextureFromImage(rlImg)
			rl.UnloadImage(rlImg)
			defer rl.UnloadTexture(textures[i])
		}

		// (Animations meta still parsed for potential frame durations; not integrated fully yet)
		_, _ = animations.New(a)

		gamParser, err := gam.NewParser()
		if err != nil {
			fmt.Println(err)
			os.Exit(1)
		}
		err = gamParser.LoadPath("assets/savegames/lastgame.gam")

		if err != nil {
			fmt.Println(err)
			os.Exit(1)
		}
		err = gamParser.Parse(buildings)
		if err != nil {
			fmt.Println(err)
			os.Exit(1)
		}

		game := &Game{textures: textures}
		game.initV2() // placeholder entity until island loader implemented
		game.gameLoopV2()
	},
}

func Execute() {
	if err := rootCmd.Execute(); err != nil {
		fmt.Println(err)
		os.Exit(1)
	}

}

func (g *Game) DrawUsage() {
	rl.DrawText("Q/E: Rotate world", 10, int32(ScreenHeight-20), 10, rl.White)
}
