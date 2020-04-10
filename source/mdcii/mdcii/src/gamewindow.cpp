
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

#ifndef _GAMEWINDOW_H_
#define _GAMEWINDOW_H_

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

#include "bildspeicher_pal8.hpp"
#include "cod_parser.hpp"
#include "files.hpp"
#include "fps.hpp"
#include "gamewindow.hpp"
#include "kamera.hpp"
#include "palette.hpp"
#include "spielbildschirm.hpp"

using namespace sdlgui;

GameWindow::GameWindow(
    SDL_Renderer* renderer, std::shared_ptr<Haeuser> haeuser, SDL_Window* pwindow, int rwidth, int rheight, const std::string& gam_name, bool fullscreen)
  : renderer(renderer)
  , haeuser(haeuser)
  , width(rwidth)
  , height(rheight)
  , gam_name(gam_name)
  , fullscreen(fullscreen)
  , running(true)
  , Screen(pwindow, Vector2i(rwidth, rheight), "Game")
{
  std::cout << "Haeuser: " << haeuser->get_haeuser_size() << std::endl;
  {
    auto& button1 = wdg<Button>("Exit", [this] {
      std::cout << "Leaving game" << std::endl;
      this->running = false;
    });
    button1.setSize(sdlgui::Vector2i{100, 20});
    button1.setPosition(rwidth - 50, rheight - 30);
  }

  performLayout(mSDL_Renderer);
}

// todo: add signal/slot for exiting window
void GameWindow::Handle()
{
  auto palette = Palette::instance();
  SDL_Texture* texture;
  SDL_Surface* final_surface;

  SDL_Surface* s8 = SDL_CreateRGBSurface(0, width, height, 8, 0, 0, 0, 0);
  SDL_Color c[palette->size()];
  int i, j;
  for (i = 0, j = 0; i < palette->size(); i++)
  {
    c[i].r = palette->getColor(i).getRed();
    c[i].g = palette->getColor(i).getGreen();
    c[i].b = palette->getColor(i).getBlue();
  }
  SDL_SetPaletteColors(s8->format->palette, c, 0, palette->size());

  std::ifstream f;
  f.open(gam_name, std::ios_base::in | std::ios_base::binary);

  Welt welt = Welt(f, haeuser);

  f.close();
  Bildspeicher_pal8 bs(width, height, 0, static_cast<uint8_t*>(s8->pixels), (uint32_t)s8->pitch);

  Spielbildschirm spielbildschirm(bs, haeuser);
  spielbildschirm.zeichne_bild(welt, 0, 0);

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
    while (running)
    {
      while (SDL_PollEvent(&e) != 0)
      {
        switch (e.type)
        {
          case SDL_QUIT:
            running = false;
            break;
          case SDL_USEREVENT:
            break;
          case SDL_KEYDOWN:
            if (e.key.keysym.sym == SDLK_F2)
            {
              spielbildschirm.kamera->setze_vergroesserung(0);
            }
            if (e.key.keysym.sym == SDLK_F3)
            {
              spielbildschirm.kamera->setze_vergroesserung(1);
            }
            if (e.key.keysym.sym == SDLK_F4)
            {
              spielbildschirm.kamera->setze_vergroesserung(2);
            }
            if (e.key.keysym.sym == SDLK_x)
            {
              spielbildschirm.kamera->rechts_drehen();
            }
            if (e.key.keysym.sym == SDLK_y)
            {
              spielbildschirm.kamera->links_drehen();
            }
            break;
        }
        this->onEvent(e);
      }
      int x, y;
      SDL_GetMouseState(&x, &y);

      if (keystate[SDL_SCANCODE_LEFT] || (fullscreen && x == 0))
      {
        spielbildschirm.kamera->nach_links();
      }
      if (keystate[SDL_SCANCODE_RIGHT] || (fullscreen && x == width - 1))
      {
        spielbildschirm.kamera->nach_rechts();
      }
      if (keystate[SDL_SCANCODE_UP] || (fullscreen && y == 0))
      {
        spielbildschirm.kamera->nach_oben();
      }
      if (keystate[SDL_SCANCODE_DOWN] || (fullscreen && y == height - 1))
      {
        spielbildschirm.kamera->nach_unten();
      }

      welt.simulationsschritt();
      spielbildschirm.zeichne_bild(welt, x, y);
      final_surface = SDL_ConvertSurfaceFormat(s8, SDL_PIXELFORMAT_RGB888, 0);
      texture = SDL_CreateTextureFromSurface(renderer, final_surface);
      SDL_FreeSurface(final_surface);
      SDL_RenderClear(renderer);
      SDL_RenderCopy(renderer, texture, NULL, NULL);
      this->drawAll();
      SDL_RenderPresent(renderer);
      SDL_DestroyTexture(texture);
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
  SDL_FreeSurface(s8);
}

#endif