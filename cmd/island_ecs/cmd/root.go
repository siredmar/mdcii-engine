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
	"github.com/siredmar/mdcii-engine/pkg/chunks"
	"github.com/siredmar/mdcii-engine/pkg/cod"
	buildingsCod "github.com/siredmar/mdcii-engine/pkg/cod/buildings"
	myecs "github.com/siredmar/mdcii-engine/pkg/ecs"
	"github.com/siredmar/mdcii-engine/pkg/files"
	"github.com/siredmar/mdcii-engine/pkg/gam"
	"github.com/siredmar/mdcii-engine/pkg/texture/atlas"
	"github.com/siredmar/mdcii-engine/pkg/world/rotation"
	"github.com/siredmar/mdcii-engine/pkg/world/zoom"
	"github.com/spf13/cobra"
	"github.com/yohamta/donburi"
	ecsdispatcher "github.com/yohamta/donburi/ecs"
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
	textures   []rl.Texture2D
	world      donburi.World // changed from *donburi.World to interface type
	dispatcher *ecsdispatcher.ECS
	island     *chunks.Island5
	atlas      *atlas.TextureAtlas // added atlas reference
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

		game := &Game{textures: textures, island: gamParser.Islands5[0], atlas: a}
		game.initECS()
		game.run()
	},
}

func Execute() {
	if err := rootCmd.Execute(); err != nil {
		fmt.Println(err)
		os.Exit(1)
	}

}

// initECS sets up Donburi world, systems, and spawns minimal entities.
func (g *Game) initECS() {
	g.world = donburi.NewWorld()
	g.dispatcher = ecsdispatcher.NewECS(g.world)
	myecs.Register(g.dispatcher) // updated call
	// Island singleton using actual island size if available
	width, height := 64, 64
	if g.island != nil {
		width = g.island.Width
		height = g.island.Height
	}
	islandEnt := g.dispatcher.Create(ecsdispatcher.LayerDefault, myecs.Island)
	islandEntry := g.world.Entry(islandEnt)
	myecs.Island.Set(islandEntry, &myecs.IslandData{Width: width, Height: height, GlobalRot: 0})
	camEnt := g.dispatcher.Create(ecsdispatcher.LayerDefault, myecs.Camera)
	camEntry := g.world.Entry(camEnt)
	myecs.Camera.Set(camEntry, &myecs.CameraData{Zoom: 1})
	// Spawn entities from island top layer fields (using atlas metadata)
	var spawned int
	if g.island != nil && g.island.Layers.Top != nil {
		for _, f := range g.island.Layers.Top.Fields {
			if f.Id == 0 || f.Id == 0xFFFF {
				continue
			}
			set := g.atlas.ImagesMeta[f.Id]
			if set == nil {
				continue
			}
			rot := rotation.Rotation(f.Orientation & 3)
			anim := set.Animations[rot]
			if anim == nil || len(anim.Images) == 0 {
				continue
			}
			frameIdx := f.AnimationCount
			if frameIdx >= len(anim.Images) {
				frameIdx = 0
			}
			meta := anim.Images[frameIdx].Metadata
			ent := g.dispatcher.Create(ecsdispatcher.LayerDefault, myecs.Transform, myecs.Render, myecs.BuildingRef)
			entry := g.world.Entry(ent)
			myecs.Transform.Set(entry, &myecs.TransformData{GridX: f.Posx, GridY: f.Posy, LocalRot: uint8(f.Orientation), Dirty: true})
			myecs.BuildingRef.Set(entry, &myecs.BuildingRefData{ID: f.Id, VariantIdx: 0})
			// Anchor: place bottom-center of sprite onto tile center; isoProject yields (x-y)*tileW/2,(x+y)*tileH/2 with tileH=tileW/2 => tileH/2 in formula
			// world tile center at projection point; sprite top-left needed -> offsetX = -Width/2, offsetY = -Height + tileHeight/2
			anchorOffsetX := -float32(meta.Width) / 2
			anchorOffsetY := -float32(meta.Height) + float32(zoom.TileSize())/4 // tileHeight/2 = tileW/4
			src := rl.Rectangle{X: float32(meta.X), Y: float32(meta.Y), Width: float32(meta.Width), Height: float32(meta.Height)}
			myecs.Render.Set(entry, &myecs.RenderData{AtlasImageIdx: meta.PNGIndex, Src: src, AnchorOffsetX: anchorOffsetX, AnchorOffsetY: anchorOffsetY})
			spawned++
		}
		myecs.MarkResortNeededForNewEntity()
	}
	// Center camera on island midpoint
	if spawned > 0 {
		midX := width / 2
		midY := height / 2
		// project midpoint (reuse simple isometric formula identical to ecs.isoProject)
		tileW := float32(zoom.TileSize())
		hw := tileW / 2
		hh := tileW / 4
		worldMidX := float32(midX-midY) * hw
		worldMidY := float32(midX+midY) * hh
		cam := myecs.Camera.Get(camEntry)
		cam.OffsetX = float32(ScreenWidth) / 2
		cam.OffsetY = float32(ScreenHeight) / 2
		cam.X = float64(worldMidX)
		cam.Y = float64(worldMidY)
		// mark all transforms dirty for repositioning
		myecs.QRenderableEach(g.world, func(tr *myecs.TransformData) { tr.Dirty = true })
	}
}

// run main loop.
func (g *Game) run() {
	last := time.Now()
	for !rl.WindowShouldClose() {
		now := time.Now()
		dt := float32(now.Sub(last).Seconds())
		last = now
		myecs.SetDeltaTime(dt)
		g.dispatcher.Update()
		g.draw()
	}
}

func (g *Game) draw() {
	rl.BeginDrawing()
	rl.ClearBackground(rl.Black)
	// Render pass
	for _, e := range myecs.SortedRenderableEntities(g.dispatcher) { // updated call
		entry := g.world.Entry(e)
		tr := myecs.Transform.Get(entry)
		rd := myecs.Render.Get(entry)
		tex := g.textures[rd.AtlasImageIdx]
		dst := rl.Rectangle{X: tr.WorldX, Y: tr.WorldY, Width: rd.Src.Width, Height: rd.Src.Height}
		origin := rl.Vector2{X: 0, Y: 0}
		rl.DrawTexturePro(tex, rd.Src, dst, origin, 0, rl.White)
	}
	g.DrawUsage()
	rl.EndDrawing()
}

func (g *Game) DrawUsage() {
	rl.DrawText("Q/E: Rotate world", 10, int32(ScreenHeight-20), 10, rl.White)
}
