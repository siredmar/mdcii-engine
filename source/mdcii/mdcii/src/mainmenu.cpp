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
  {
    auto& window = wdg<Window>("Grid of small widgets");
    window.withPosition({425, 288});
    auto* layout = new GridLayout(Orientation::Horizontal, 2, Alignment::Middle, 15, 5);
    layout->setColAlignment({Alignment::Maximum, Alignment::Fill});
    layout->setSpacing(50, 10);
    window.setLayout(layout);

    window.add<Label>("Floating point :", "sans-bold");
    auto& textBox = window.wdg<TextBox>();
    textBox.setEditable(true);
    textBox.setFixedSize(Vector2i(100, 20));
    textBox.setValue("50");
    textBox.setUnits("GiB");
    textBox.setDefaultValue("0.0");
    textBox.setFontSize(16);
    textBox.setFormat("[-]?[0-9]*\\.?[0-9]+");

    window.add<Label>("Positive integer :", "sans-bold");
    auto& textBox2 = window.wdg<TextBox>();
    textBox2.setEditable(true);
    textBox2.setFixedSize(Vector2i(100, 20));
    textBox2.setValue("50");
    textBox2.setUnits("Mhz");
    textBox2.setDefaultValue("0.0");
    textBox2.setFontSize(16);
    textBox2.setFormat("[1-9][0-9]*");

    window.add<Label>("Checkbox :", "sans-bold");

    window.wdg<CheckBox>("Check me").withChecked(true).withFontSize(16);

    window.add<Label>("Combo box :", "sans-bold");
    window.wdg<ComboBox>().withItems(std::vector<std::string>{"Item 1", "Item 2", "Item 3"}).withFontSize(16).withFixedSize(Vector2i(100, 20));

    window.add<Label>("Color button :", "sans-bold");
    auto& popupBtn = window.wdg<PopupButton>("", 0);
    popupBtn.setBackgroundColor(Color(255, 120, 0, 255));
    popupBtn.setFontSize(16);
    popupBtn.setFixedSize(Vector2i(100, 20));
    auto& popup = popupBtn.popup().withLayout<GroupLayout>();

    ColorWheel& colorwheel = popup.wdg<ColorWheel>();
    colorwheel.setColor(popupBtn.backgroundColor());

    Button& colorBtn = popup.wdg<Button>("Pick");
    colorBtn.setFixedSize(Vector2i(100, 25));
    Color c = colorwheel.color();
    colorBtn.setBackgroundColor(c);

    colorwheel.setCallback([&colorBtn](const Color& value) { colorBtn.setBackgroundColor(value); });

    colorBtn.setChangeCallback([&colorBtn, &popupBtn](bool pushed) {
      if (pushed)
      {
        popupBtn.setBackgroundColor(colorBtn.backgroundColor());
        popupBtn.setPushed(false);
      }
    });
  }

  performLayout(mSDL_Renderer);
}