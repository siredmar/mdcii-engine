#include "mainmenu.hpp"
#include "SDL2/SDL.h"

using namespace sdlgui;

MainMenu::MainMenu(const std::string& basegad_path, SDL_Window* pwindow, int rwidth, int rheight)
  : path(basegad_path)
  , cod(std::make_shared<Cod_Parser>(basegad_path, false, false))
  , basegad(std::make_shared<Basegad>(cod))
// , Screen(pwindow, Vector2i(rwidth, rheight), "SDL_gui Test")
{
}

void MainMenu::Show()

{
  std::cout << "Basegad: " << basegad->get_gadgets_size() << std::endl;
  // {
  //   auto& nwindow = window("Button demo", Vector2i{15, 15}).withLayout<GroupLayout>();

  //   nwindow.label("Push buttons", "sans-bold")._and().button("Plain button", [] { cout << "pushed!" << endl; }).withTooltip("This is plain button tips");

  //   nwindow.button("Styled", ENTYPO_ICON_ROCKET, [] { cout << "pushed!" << endl; }).withBackgroundColor(Color(0, 0, 255, 25));

  //   nwindow.label("Toggle buttons", "sans-bold")
  //       ._and()
  //       .button("Toggle me", [](bool state) { cout << "Toggle button state: " << state << endl; })
  //       .withFlags(Button::ToggleButton);

  //   nwindow.label("Radio buttons", "sans-bold")._and().button("Radio button 1").withFlags(Button::RadioButton);

  //   nwindow.button("Radio button 2").withFlags(Button::RadioButton)._and().label("A tool palette", "sans-bold");

  //   auto& tools = nwindow.widget().boxlayout(Orientation::Horizontal, Alignment::Middle, 0, 6);

  //   tools.toolbutton(ENTYPO_ICON_CLOUD)._and().toolbutton(ENTYPO_ICON_FF)._and().toolbutton(ENTYPO_ICON_COMPASS)._and().toolbutton(ENTYPO_ICON_INSTALL);

  //   nwindow.label("Popup buttons", "sans-bold")
  //       ._and()
  //       .popupbutton("Popup", ENTYPO_ICON_EXPORT)
  //       .popup()
  //       .withLayout<GroupLayout>()
  //       .label("Arbitrary widgets can be placed here")
  //       ._and()
  //       .checkbox("A check box")
  //       ._and()
  //       .popupbutton("Recursive popup", ENTYPO_ICON_FLASH)
  //       .popup()
  //       .withLayout<GroupLayout>()
  //       .checkbox("Another check box");
  // }
}