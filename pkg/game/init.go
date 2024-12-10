package game

// import (
// 	"fmt"
// 	"os"
// 	"path/filepath"

// 	"github.com/siredmar/mdcii-engine/pkg/bsh"
// 	"github.com/siredmar/mdcii-engine/pkg/cod"
// 	"github.com/siredmar/mdcii-engine/pkg/cod/buildings"
// 	"github.com/siredmar/mdcii-engine/pkg/files"
// 	"github.com/siredmar/mdcii-engine/pkg/gam"
// 	"github.com/siredmar/mdcii-engine/pkg/texture/atlas"
// 	"github.com/siredmar/mdcii-engine/pkg/texture/sprites"
// )

// func (g *Game) Init() error {
// 	absPath, err := filepath.Abs(g.Config.GamePath)
// 	if err != nil {
// 		fmt.Println(err)
// 		os.Exit(1)
// 	}

// 	dirPath := filepath.Dir(absPath)

// 	files.CreateInstance(dirPath)
// 	buildingsCodPath, err := files.Instance().FindPathForFile("haeuser.cod")
// 	if err != nil {
// 		fmt.Println(err)
// 		os.Exit(1)
// 	}
// 	haeuserCod, err := cod.NewCod(buildingsCodPath, true)
// 	if err != nil {
// 		fmt.Println(err)
// 		os.Exit(1)
// 	}
// 	err = haeuserCod.Parse()
// 	if err != nil {
// 		fmt.Println(err)
// 		os.Exit(1)
// 	}

// 	buildings, err := buildings.NewBuildings(haeuserCod)
// 	if err != nil {
// 		fmt.Println(err)
// 		os.Exit(1)
// 	}

// 	gamParser, err := gam.NewParser()
// 	if err != nil {
// 		fmt.Println(err)
// 		os.Exit(1)
// 	}
// 	err = gamParser.LoadPath(c.Config.GamFile)
// 	if err != nil {
// 		fmt.Println(err)
// 		os.Exit(1)
// 	}
// 	err = gamParser.Parse(buildings)
// 	if err != nil {
// 		fmt.Println(err)
// 		os.Exit(1)
// 	}

// 	gfxStadtfldBshPath, err := files.Instance().FindPathForFile("gfx/stadtfld.bsh")
// 	if err != nil {
// 		fmt.Println(err)
// 		os.Exit(1)
// 	}
// 	gfxStadtfldBsh, err := bsh.NewPng(bsh.WithFile(gfxStadtfldBshPath), bsh.WithConvertAll())
// 	if err != nil {
// 		fmt.Println(err)
// 		os.Exit(1)
// 	}

// 	gridAtlas, err := atlas.New(100, 100, atlas.WithName("grid"), atlas.WithFiles([]string{"./assets/gfx/0.png", "./assets/gfx/1.png"}))
// 	if err != nil {
// 		fmt.Println(err)
// 		os.Exit(1)
// 	}

// 	gfxAtlas, err := atlas.New(4096, 4096, atlas.WithName("gfx-stadtfld"), atlas.WithImages(gfxStadtfldBsh.Images))
// 	if err != nil {
// 		fmt.Println(err)
// 		os.Exit(1)
// 	}

// 	gridSprites, err := sprites.NewSprites(gridAtlas)
// 	if err != nil {
// 		fmt.Println(err)
// 		os.Exit(1)
// 	}

// 	gfxSprites, err := sprites.NewSprites(gfxAtlas)
// 	if err != nil {
// 		fmt.Println(err)
// 		os.Exit(1)
// 	}
// }
