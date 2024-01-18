package main

import (
	"log"

	ebiten "github.com/hajimehoshi/ebiten/v2"
	"github.com/siredmar/mdcii-engine/pkg/game"
)

func main() {
	ebiten.SetWindowTitle("MDCII")
	ebiten.SetWindowSize(640, 480)
	ebiten.SetWindowResizable(true)

	g, err := game.NewGame()
	if err != nil {
		log.Fatal(err)
	}

	if err = ebiten.RunGame(g); err != nil {
		log.Fatal(err)
	}
}
