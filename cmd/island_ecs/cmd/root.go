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
	"math"
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
	buildings  *buildingsCod.Buildings
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

		game := &Game{textures: textures, island: gamParser.Islands5[0], atlas: a, buildings: buildings}
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
	// Spawn entities from island top layer fields (placeholder rendering)
	if g.island != nil && g.island.Layers.Top != nil {
		for _, f := range g.island.Layers.Top.Fields {
			if f.Id == 0 || f.Id == 0xFFFF {
				continue
			}
			if f.Posx == 14 && f.Posy == 22 {
				fmt.Println("14x22")
			}
			if f.Posx == 15 && f.Posy == 22 {
				fmt.Println("15x22")
			}
			if f.Posx == 16 && f.Posy == 22 {
				fmt.Println("16x22")
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
			myecs.Transform.Set(entry, &myecs.TransformData{GridX: 0, GridY: 0, LocalRot: uint8(f.Orientation), Dirty: true})
			myecs.BuildingRef.Set(entry, &myecs.BuildingRefData{ID: f.Id, VariantIdx: 0})

			bld := g.buildings.Buildings[f.Id]
			if bld == nil {
				continue
			}
			if bld.Size.H > 1 || bld.Size.W > 1 {
				continue
			}

			sizeX, sizeY := bld.Size.W, bld.Size.H
			if rot == rotation.DEG90 || rot == rotation.DEG270 {
				sizeX, sizeY = sizeY, sizeX
			}
			tileW := float32(zoom.TileSize()) * 2 // atlas tiles are twice the zoom size
			tileH := tileW / 2

			// determine anchor offsets depending on rotation; anchor tile position moves
			anchorOffsetX := float32(0)
			// base offset so sprite's bottom rests on tile baseline (isoProject returns tile center)
			anchorOffsetY := -float32(meta.Height) + tileH/2
			switch rot {
			case rotation.DEG0:
				anchorOffsetX -= float32(sizeY-1) * tileW / 2
				anchorOffsetY -= float32(sizeX-1) * tileH / 2
			case rotation.DEG90:
				anchorOffsetX += float32(sizeX-1) * tileW / 2
				anchorOffsetY -= float32(sizeY-1) * tileH / 2
			case rotation.DEG180:
				anchorOffsetX += float32(sizeY-1) * tileW / 2
				anchorOffsetY += float32(sizeX-1) * tileH / 2
			case rotation.DEG270:
				anchorOffsetX -= float32(sizeX-1) * tileW / 2
				anchorOffsetY += float32(sizeY-1) * tileH / 2
			}
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
		tileW := float32(zoom.TileSize()) * 2
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
	g.drawGrid()
	g.drawMouseCoords()
	g.DrawUsage()
	rl.EndDrawing()
}

func (g *Game) drawMouseCoords() {
	_, cam := myecs.CameraEntity(g.world)
	if cam == nil {
		return
	}
	mx := float32(rl.GetMouseX())
	my := float32(rl.GetMouseY())
	z := cam.Zoom
	worldX := (mx-cam.OffsetX)/z + float32(cam.X)
	worldY := (my-cam.OffsetY)/z + float32(cam.Y)
	tileW := float32(zoom.TileSize()) * 2
	hw := tileW / 2
	hh := tileW / 4
	gx := int(math.Floor(float64((worldX/hw + worldY/hh) / 2)))
	gy := int(math.Floor(float64((worldY/hh - worldX/hw) / 2)))
	rl.DrawText(fmt.Sprintf("%d,%d", gx, gy), 10, 10, 10, rl.White)
}

func (g *Game) DrawUsage() {
	rl.DrawText("Q/E: Rotate world", 10, int32(ScreenHeight-20), 10, rl.White)
}

// drawGrid overlays an isometric grid so sprite alignment can be verified visually.
func (g *Game) drawGrid() {
	_, island := myecs.IslandEntity(g.world)
	if island == nil {
		return
	}
	_, cam := myecs.CameraEntity(g.world)
	if cam == nil {
		cam = &myecs.CameraData{Zoom: 1}
	}
	tileW := float32(zoom.TileSize()) * 2
	tileH := tileW / 2
	hw := tileW / 2
	hh := tileH / 2
	for y := 0; y < island.Height; y++ {
		for x := 0; x < island.Width; x++ {
			bx, by := isoProject(x, y, tileW)
			// diamond vertices relative to bottom point
			lx, ly := bx-hw, by-hh
			rx, ry := bx+hw, by-hh
			tx, ty := bx, by-tileH
			// camera transform
			z := cam.Zoom
			drawLine := func(x1, y1, x2, y2 float32) {
				sx1 := cam.OffsetX + (x1-float32(cam.X))*z
				sy1 := cam.OffsetY + (y1-float32(cam.Y))*z
				sx2 := cam.OffsetX + (x2-float32(cam.X))*z
				sy2 := cam.OffsetY + (y2-float32(cam.Y))*z
				rl.DrawLineEx(rl.NewVector2(sx1, sy1), rl.NewVector2(sx2, sy2), 1, rl.Red)
			}
			drawLine(bx, by, lx, ly)
			drawLine(lx, ly, tx, ty)
			drawLine(tx, ty, rx, ry)
			drawLine(rx, ry, bx, by)
		}
	}
}

// isoProject converts grid coordinates to world coordinates (bottom vertex of tile).
func isoProject(gx, gy int, tileW float32) (float32, float32) {
	hw := tileW / 2
	hh := tileW / 4
	x := (float32(gx - gy)) * hw
	y := (float32(gx + gy)) * hh
	return x, y
}
