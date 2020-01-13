#include <iostream>

#include "mainmenu.hpp"
#include "SDL2/SDL.h"
#include "sdlgui/screen.h"
#include "sdlgui/window.h"
#include "sdlgui/layout.h"
#include "sdlgui/label.h"
#include "sdlgui/checkbox.h"
#include "sdlgui/button.h"
#include "sdlgui/toolbutton.h"
#include "sdlgui/popupbutton.h"
#include "sdlgui/combobox.h"
#include "sdlgui/dropdownbox.h"
#include "sdlgui/progressbar.h"
#include "sdlgui/entypo.h"
#include "sdlgui/messagedialog.h"
#include "sdlgui/textbox.h"
#include "sdlgui/slider.h"
#include "sdlgui/imagepanel.h"
#include "sdlgui/imageview.h"
#include "sdlgui/vscrollpanel.h"
#include "sdlgui/colorwheel.h"
#include "sdlgui/graph.h"
#include "sdlgui/tabwidget.h"
#include "sdlgui/switchbox.h"
#include "sdlgui/formhelper.h"

using namespace sdlgui;

MainMenu::MainMenu(const std::string& basegad_path, SDL_Window* pwindow, int rwidth, int rheight)
  : path(basegad_path)
  , cod(std::make_shared<Cod_Parser>(basegad_path, false, false))
  , basegad(std::make_shared<Basegad>(cod))
  , Screen(pwindow, Vector2i(rwidth, rheight), "SDL_gui Test")
{
  std::cout << "Basegad: " << basegad->get_gadgets_size() << std::endl;
  {
    auto& label = wdg<Label>("Tesetlabel", "sans-bold");
    label.setPosition(100, 500);
    auto& button1 = wdg<Button>("Button", [] { std::cout << "clicked" << std::endl; });
    button1.setPosition(200, 200);
  }

  performLayout(mSDL_Renderer);
}