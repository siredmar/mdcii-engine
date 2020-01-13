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
// #include "sdlgui/"

using namespace sdlgui;

MainMenu::MainMenu(const std::string& basegad_path, SDL_Window* pwindow, int rwidth, int rheight)
  : path(basegad_path)
  , cod(std::make_shared<Cod_Parser>(basegad_path, false, false))
  , basegad(std::make_shared<Basegad>(cod))
  , Screen(pwindow, Vector2i(rwidth, rheight), "SDL_gui Test")
{
  std::cout << "Basegad: " << basegad->get_gadgets_size() << std::endl;
  // {
  //   auto& nwindow = window("Button demo", Vector2i{15, 15}).withLayout<GroupLayout>();

  //   nwindow.label("Push buttons", "sans-bold")
  //       ._and()
  //       .button("Plain button", [] { std::cout << "pushed!" << std::endl; })
  //       .withTooltip("This is plain button tips");

  //   nwindow.button("Styled", ENTYPO_ICON_ROCKET, [] { std::cout << "pushed!" << std::endl; }).withBackgroundColor(Color(0, 0, 255, 25));

  //   nwindow.label("Toggle buttons", "sans-bold")
  //       ._and()
  //       .button("Toggle me", [](bool state) { std::cout << "Toggle button state: " << state << std::endl; })
  //       .withFlags(Button::ToggleButton);

  //   nwindow.label("Radio buttons", "sans-bold")._and().button("Radio button 1").withFlags(Button::RadioButton);

  //   nwindow.button("Radio button 2").withFlags(Button::RadioButton)._and().label("A tool palette", "sans-bold");

  //   auto& tools = nwindow.widget().boxlayout(Orientation::Horizontal, Alignment::Middle, 0, 6);

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

  {
    AdvancedGridLayout* layout = new AdvancedGridLayout();
    this->setLayout(layout);
    layout->appendRow(100, 1);
    layout->appendCol(100, 1);
    auto& nwindow = window("Button demo", Vector2i{15, 15}).withLayout<AdvancedGridLayout>();
    nwindow.setLayout(layout);
    Button* b1 = new Button(this, "b1");
    layout->setAnchor(b1, AdvancedGridLayout::Anchor(0, 0));

    // nwindow.label("Push buttons", "sans-bold")
    //     ._and()
    //     .button("Plain button", [] { std::cout << "pushed!" << std::endl; })
    //     .withTooltip("This is plain button tips");
    // nwindow.button("Styled", ENTYPO_ICON_ROCKET, [] { std::cout << "pushed!" << std::endl; }).withBackgroundColor(Color(0, 0, 255, 25));

    // nwindow.label("Toggle buttons", "sans-bold")
    //     ._and()
    //     .button("Toggle me", [](bool state) { std::cout << "Toggle button state: " << state << std::endl; })
    //     .withFlags(Button::ToggleButton);

    // nwindow.label("Radio buttons", "sans-bold")._and().button("Radio button 1").withFlags(Button::RadioButton);

    // nwindow.button("Radio button 2").withFlags(Button::RadioButton)._and().label("A tool palette", "sans-bold");

    // auto& tools = nwindow.widget().boxlayout(Orientation::Horizontal, Alignment::Middle, 0, 6);

    // nwindow.label("Popup buttons", "sans-bold")
    //     ._and()
    //     .popupbutton("Popup", ENTYPO_ICON_EXPORT)
    //     .popup()
    //     .withLayout<GroupLayout>()
    //     .label("Arbitrary widgets can be placed here")
    //     ._and()
    //     .checkbox("A check box")
    //     ._and()
    //     .popupbutton("Recursive popup", ENTYPO_ICON_FLASH)
    //     .popup()
    //     .withLayout<GroupLayout>()
    //     .checkbox("Another check box");
  }

  // {
  //   AdvancedGridLayout* layout = new AdvancedGridLayout();
  //   this->setLayout(layout);

  //   layout->appendRow(100, 0);
  //   layout->appendRow(0, 1);
  //   layout->appendCol(0, 1);
  //   Button* b1 = new Button(this, "b1");
  //   layout->setAnchor(b1, AdvancedGridLayout::Anchor(0, 0));

  //   Widget* panel = new Widget(this);
  //   layout->setAnchor(panel, AdvancedGridLayout::Anchor(0, 1));

  //   AdvancedGridLayout* layout2 = new AdvancedGridLayout();
  //   panel->setLayout(layout2);

  //   layout2->appendCol(100, 0);
  //   layout2->appendCol(0, 1);
  //   layout2->appendRow(0, 1);

  //   Button* b2 = new Button(panel, "b2");
  //   layout2->setAnchor(b2, AdvancedGridLayout::Anchor(0, 0));

  //   Button* b3 = new Button(panel, "b3");
  //   layout2->setAnchor(b3, AdvancedGridLayout::Anchor(1, 0));
  // }
  performLayout(mSDL_Renderer);
}