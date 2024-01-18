package game

import (
	"fmt"
	"math"

	"image/color"

	"github.com/ebitenui/ebitenui"
	"github.com/ebitenui/ebitenui/image"
	"github.com/ebitenui/ebitenui/widget"
	ebiten "github.com/hajimehoshi/ebiten/v2"
	ebitenutil "github.com/hajimehoshi/ebiten/v2/ebitenutil"
)

// Game is an isometric demo game.
type Game struct {
	width      int
	height     int
	camX       float64
	camY       float64
	camScale   float64
	camScaleTo float64
	mousePanX  int
	mousePanY  int
	offscreen  *ebiten.Image

	ui *ebitenui.UI
}

// NewGame returns a new isometric demo Game.
func NewGame() (*Game, error) {
	buttonImage := loadButtonImage()
	buttonIcon := loadButtonIcon()

	// construct a new container that serves as the root of the UI hierarchy
	rootContainer := widget.NewContainer(
		// the container will use a plain color as its background
		widget.ContainerOpts.BackgroundImage(image.NewNineSliceColor(color.RGBA{0x13, 0x1a, 0x22, 0xff})),

		// the container will use an anchor layout to layout its single child widget
		widget.ContainerOpts.Layout(widget.NewAnchorLayout()),
	)

	// We can achieve a button with image instead of text by using a combination of
	// normal button (without text) and graphics widget.
	// We bundle them together using a stacked layout container.
	buttonStackedLayout := widget.NewContainer(
		widget.ContainerOpts.Layout(widget.NewStackedLayout()),
		// instruct the container's anchor layout to center the button both horizontally and vertically;
		// since our button is a 2-widget object, we add the anchor info to the wrapping container
		// instead of the button
		widget.ContainerOpts.WidgetOpts(widget.WidgetOpts.LayoutData(widget.AnchorLayoutData{
			HorizontalPosition: widget.AnchorLayoutPositionCenter,
			VerticalPosition:   widget.AnchorLayoutPositionCenter,
		})),
	)
	// construct a pressable button
	button := widget.NewButton(
		// specify the images to use
		widget.ButtonOpts.Image(buttonImage),

		// add a handler that reacts to clicking the button
		widget.ButtonOpts.ClickedHandler(func(args *widget.ButtonClickedEventArgs) {
			println("button clicked")
		}),
	)
	buttonStackedLayout.AddChild(button)
	// Put an image on top of the button, it will be centered.
	// If your image doesn't fit the button and there is no Y stretching support,
	// you may see a transparent rectangle inside the button.
	// To fix that, either use a separate button image (that can fit the image)
	// or add an appropriate stretching.
	buttonStackedLayout.AddChild(widget.NewGraphic(widget.GraphicOpts.Image(buttonIcon)))

	// since our button is a multi-widget object, add its wrapping container
	rootContainer.AddChild(buttonStackedLayout)

	// construct the UI
	ui := ebitenui.UI{
		Container: rootContainer,
	}

	g := &Game{
		camScale:   1,
		camScaleTo: 1,
		mousePanX:  math.MinInt32,
		mousePanY:  math.MinInt32,
		ui:         &ui,
	}
	return g, nil
}

func loadButtonIcon() *ebiten.Image {
	// we'll use a circle as an icon image
	// in reality it could be an arbitrary *ebiten.Image
	icon := ebiten.NewImage(32, 32)
	ebitenutil.DrawCircle(icon, 16, 16, 16, color.RGBA{R: 0x71, G: 0x56, B: 0xbd, A: 255})
	return icon
}

func loadButtonImage() *widget.ButtonImage {
	idle := image.NewNineSliceColor(color.RGBA{R: 170, G: 170, B: 180, A: 255})
	hover := image.NewNineSliceColor(color.RGBA{R: 130, G: 130, B: 150, A: 255})
	pressed := image.NewNineSliceColor(color.RGBA{R: 100, G: 100, B: 120, A: 255})

	return &widget.ButtonImage{
		Idle:    idle,
		Hover:   hover,
		Pressed: pressed,
	}
}

// Update reads current user input and updates the Game state.
func (g *Game) Update() error {
	// Update target zoom level.
	var scrollY float64
	if ebiten.IsKeyPressed(ebiten.KeyC) || ebiten.IsKeyPressed(ebiten.KeyPageDown) {
		scrollY = -0.25
	} else if ebiten.IsKeyPressed(ebiten.KeyE) || ebiten.IsKeyPressed(ebiten.KeyPageUp) {
		scrollY = 0.25
	} else {
		_, scrollY = ebiten.Wheel()
		if scrollY < -1 {
			scrollY = -1
		} else if scrollY > 1 {
			scrollY = 1
		}
	}
	g.camScaleTo += scrollY * (g.camScaleTo / 7)

	// Clamp target zoom level.
	if g.camScaleTo < 0.01 {
		g.camScaleTo = 0.01
	} else if g.camScaleTo > 100 {
		g.camScaleTo = 100
	}

	// Smooth zoom transition.
	div := 10.0
	if g.camScaleTo > g.camScale {
		g.camScale += (g.camScaleTo - g.camScale) / div
	} else if g.camScaleTo < g.camScale {
		g.camScale -= (g.camScale - g.camScaleTo) / div
	}

	// Pan camera via keyboard.
	pan := 7.0 / g.camScale
	if ebiten.IsKeyPressed(ebiten.KeyLeft) || ebiten.IsKeyPressed(ebiten.KeyA) {
		g.camX -= pan
	}
	if ebiten.IsKeyPressed(ebiten.KeyRight) || ebiten.IsKeyPressed(ebiten.KeyD) {
		g.camX += pan
	}
	if ebiten.IsKeyPressed(ebiten.KeyDown) || ebiten.IsKeyPressed(ebiten.KeyS) {
		g.camY -= pan
	}
	if ebiten.IsKeyPressed(ebiten.KeyUp) || ebiten.IsKeyPressed(ebiten.KeyW) {
		g.camY += pan
	}

	fmt.Println(g.camX, g.camY)
	g.ui.Update()
	// // Pan camera via mouse.
	// if ebiten.IsMouseButtonPressed(ebiten.MouseButtonRight) {
	// 	if g.mousePanX == math.MinInt32 && g.mousePanY == math.MinInt32 {
	// 		g.mousePanX, g.mousePanY = ebiten.CursorPosition()
	// 	} else {
	// 		x, y := ebiten.CursorPosition()
	// 		dx, dy := float64(g.mousePanX-x)*(pan/100), float64(g.mousePanY-y)*(pan/100)
	// 		g.camX, g.camY = g.camX-dx, g.camY+dy
	// 	}
	// } else if g.mousePanX != math.MinInt32 || g.mousePanY != math.MinInt32 {
	// 	g.mousePanX, g.mousePanY = math.MinInt32, math.MinInt32
	// }

	// // Clamp camera position.
	// worldWidth := float64(g.currentLevel.width * g.currentLevel.tileSize / 2)
	// worldHeight := float64(g.currentLevel.height * g.currentLevel.tileSize / 2)
	// if g.camX < -worldWidth {
	// 	g.camX = -worldWidth
	// } else if g.camX > worldWidth {
	// 	g.camX = worldWidth
	// }
	// if g.camY < -worldHeight {
	// 	g.camY = -worldHeight
	// } else if g.camY > 0 {
	// 	g.camY = 0
	// }

	// // Move level up or down.
	// if inpututil.IsKeyJustPressed(ebiten.KeyV) {
	// 	g.currentLevel.LevelDown()
	// } else if inpututil.IsKeyJustPressed(ebiten.KeyF) {
	// 	g.currentLevel.LevelUp()
	// }

	// // If we click, print the tile we clicked on.
	// if inpututil.IsMouseButtonJustPressed(ebiten.MouseButtonLeft) {
	// 	x, y := getTileXY(g)
	// 	fmt.Printf("Clicked on tile %d,%d\n", x, y)
	// 	// TODO: Add pathfinding from player to tile.
	// }

	return nil
}

// Draw draws the Game on the screen.
func (g *Game) Draw(screen *ebiten.Image) {
	// Render level.
	// g.renderLevel(screen)
	g.ui.Draw(screen)
	// Print game info.
	ebitenutil.DebugPrint(screen, fmt.Sprintf("KEYS WASD EC FV R\nFPS  %0.0f\nTPS  %0.0f\nSCA  %0.2f\nPOS  %0.0f,%0.0f\n", ebiten.ActualFPS(), ebiten.ActualTPS(), g.camScale, g.camX, g.camY))
}

// Layout is called when the Game's layout changes.
func (g *Game) Layout(outsideWidth, outsideHeight int) (int, int) {
	g.width, g.height = outsideWidth, outsideHeight
	return g.width, g.height
}

// // cartesianToIso transforms cartesian coordinates into isometric coordinates.
// func (g *Game) cartesianToIso(x, y float64) (float64, float64) {
// 	tileSize := g.currentLevel.tileSize
// 	ix := (x - y) * float64(tileSize/2)
// 	iy := (x + y) * float64(tileSize/4)
// 	return ix, iy
// }

// // This function might be useful for those who want to modify this example.

// // isoToCartesian transforms isometric coordinates into cartesian coordinates.
// func (g *Game) isoToCartesian(x, y float64) (float64, float64) {
// 	tileSize := g.currentLevel.tileSize
// 	cx := (x/float64(tileSize/2) + y/float64(tileSize/4)) / 2
// 	cy := (y/float64(tileSize/4) - (x / float64(tileSize/2))) / 2
// 	return cx, cy
// }
