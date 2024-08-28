package main

import (
	"flag"
	"log"

	ebiten "github.com/hajimehoshi/ebiten/v2"
	"github.com/siredmar/mdcii-engine/pkg/game"
)

var (
	path = flag.String("path", ".", "PATH TO GAME")
)

func main() {
	flag.Parse()
	ebiten.SetWindowTitle("MDCII")
	ebiten.SetWindowSize(640, 480)
	ebiten.SetWindowResizable(true)

	g, err := game.NewGame(*path, "")
	if err != nil {
		log.Fatal(err)
	}

	if err = ebiten.RunGame(g); err != nil {
		log.Fatal(err)
	}
}
