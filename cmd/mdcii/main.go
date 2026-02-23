package main

import (
	"flag"

	ebiten "github.com/hajimehoshi/ebiten/v2"
)

var (
	path = flag.String("path", ".", "PATH TO GAME")
)

func main() {
	flag.Parse()
	ebiten.SetWindowTitle("MDCII")
	ebiten.SetWindowSize(640, 480)
	ebiten.SetWindowResizable(true)

	// g, err := game.NewGame(*path, "")
	// if err != nil {
	// 	log.Fatal(err)
	// }

	// if err = ebiten.RunGame(g); err != nil {
	// 	log.Fatal(err)
	// }
}
