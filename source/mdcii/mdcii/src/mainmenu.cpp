
// This file is part of the MDCII Game Engine.
// Copyright (C) 2020  Armin Schlegel
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include <iostream>

#include "SDL2/SDL.h"

#include "sdlgui/button.h"
#include "sdlgui/checkbox.h"
#include "sdlgui/colorwheel.h"
#include "sdlgui/combobox.h"
#include "sdlgui/dropdownbox.h"
#include "sdlgui/entypo.h"
#include "sdlgui/formhelper.h"
#include "sdlgui/graph.h"
#include "sdlgui/imagepanel.h"
#include "sdlgui/imageview.h"
#include "sdlgui/label.h"
#include "sdlgui/layout.h"
#include "sdlgui/messagedialog.h"
#include "sdlgui/popupbutton.h"
#include "sdlgui/progressbar.h"
#include "sdlgui/screen.h"
#include "sdlgui/slider.h"
#include "sdlgui/switchbox.h"
#include "sdlgui/tabwidget.h"
#include "sdlgui/textbox.h"
#include "sdlgui/toolbutton.h"
#include "sdlgui/vscrollpanel.h"
#include "sdlgui/window.h"

#include "cod_parser.hpp"
#include "files.hpp"
#include "fps.hpp"
#include "gamewindow.hpp"
#include "haeuser.hpp"
#include "mainmenu.hpp"
#include "palette.hpp"

using namespace sdlgui;
MainMenu::MainMenu(
    SDL_Renderer* renderer, std::shared_ptr<Basegad> basegad, SDL_Window* pwindow, int rwidth, int rheight, bool fullscreen, const std::string& gam_name)
  : renderer(renderer)
  , basegad(basegad)
  , width(rwidth)
  , height(rheight)
  , fullscreen(fullscreen)
  , gam_name(gam_name)
  , pwindow(pwindow)
  , files(Files::instance())
  , Screen(pwindow, Vector2i(rwidth, rheight), "Game")
{
  std::cout << "Basegad: " << basegad->get_gadgets_size() << std::endl;
  {
    auto& label = wdg<Label>("Tesetlabel", "sans-bold");
    label.setPosition(100, 500);
    auto& button1 = wdg<Button>("Button", [this] {
      std::cout << "loading game: " << this->gam_name << std::endl;
      this->LoadGame(this->gam_name);
    });
    button1.setPosition(200, 200);
  }

  performLayout(mSDL_Renderer);
}

void MainMenu::LoadGame(const std::string& gam_name)
{
  auto haeuser_cod = std::make_shared<Cod_Parser>(files->instance()->find_path_for_file("haeuser.cod"), true, false);
  auto haeuser = std::make_shared<Haeuser>(haeuser_cod);

  GameWindow gameWindow(renderer, haeuser, pwindow, width, height, gam_name, fullscreen);
  gameWindow.Handle();
  Handle();
}

// todo: add signal/slot for exiting window
void MainMenu::Handle()
{
  SDL_Texture* texture;
  SDL_Surface* final_surface;

  SDL_Surface* s8 = SDL_CreateRGBSurface(0, width, height, 8, 0, 0, 0, 0);
  SDL_Color c[256];
  int i, j;
  for (i = 0, j = 0; i < 256; i++)
  {
    c[i].r = palette[j++];
    c[i].g = palette[j++];
    c[i].b = palette[j++];
  }
  SDL_SetPaletteColors(s8->format->palette, c, 0, 255);

  try
  {

    final_surface = SDL_ConvertSurfaceFormat(s8, SDL_PIXELFORMAT_RGB888, 0);
    texture = SDL_CreateTextureFromSurface(renderer, final_surface);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    this->drawAll();
    SDL_RenderPresent(renderer);

    Fps fps;
    const Uint8* keystate = SDL_GetKeyboardState(NULL);

    SDL_Event e;
    bool quit = false;
    while (!quit)
    {
      while (SDL_PollEvent(&e) != 0)
      {
        switch (e.type)
        {
          case SDL_QUIT:
            quit = true;
            break;
          case SDL_USEREVENT:
            break;
          case SDL_KEYDOWN:
            if (e.key.keysym.sym == SDLK_ESCAPE)
            {
              quit = true;
            }
            break;
        }
        this->onEvent(e);
      }
      int x, y;
      SDL_GetMouseState(&x, &y);

      final_surface = SDL_ConvertSurfaceFormat(s8, SDL_PIXELFORMAT_RGB888, 0);
      texture = SDL_CreateTextureFromSurface(renderer, final_surface);
      SDL_RenderClear(renderer);
      SDL_RenderCopy(renderer, texture, NULL, NULL);
      this->drawAll();
      SDL_RenderPresent(renderer);
      fps.next();
    }
  }
  catch (const std::runtime_error& e)
  {
    std::string error_msg = std::string("Caught a fatal error: ") + std::string(e.what());
#if defined(_WIN32)
    MessageBoxA(nullptr, error_msg.c_str(), NULL, MB_ICONERROR | MB_OK);
#else
    std::cerr << error_msg << std::endl;
#endif
  }
}
