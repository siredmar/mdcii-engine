#ifndef _GAMEWINDOW_H_
#define _GAMEWINDOW_H_

#include <iostream>

#include "fps.hpp"
#include "palette.hpp"
#include "gamewindow.hpp"
#include "files.hpp"
#include "kamera.hpp"
#include "bildspeicher_pal8.hpp"
#include "spielbildschirm.hpp"
#include "cod_parser.hpp"

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

GameWindow::GameWindow(
    SDL_Renderer* renderer, const std::string& haeuser_cod, SDL_Window* pwindow, int rwidth, int rheight, const std::string& gam_name, bool fullscreen)
  : renderer(renderer)
  , cod(std::make_shared<Cod_Parser>(haeuser_cod, true, false))
  , haeuser(std::make_shared<Haeuser>(cod))
  , width(rwidth)
  , height(rheight)
  , gam_name(gam_name)
  , fullscreen(fullscreen)
  , Screen(pwindow, Vector2i(rwidth, rheight), "Game")
{
  std::cout << "Haeuser: " << haeuser->get_haeuser_size() << std::endl;
  {
    auto& button1 = wdg<Button>("Button", [] { std::cout << "clicked" << std::endl; });
    button1.setPosition(rwidth - 250, rheight - 40);
  }

  performLayout(mSDL_Renderer);
}

// todo: add signal/slot for exiting window
void GameWindow::Handle()
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
    bool quit = false;
    while (!quit)
    {
      while (SDL_PollEvent(&e) != 0)
      {
        switch (e.type)
        {
          this->onEvent(e);
          case SDL_QUIT: quit = true; break;
          case SDL_USEREVENT: break;
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
            if (e.key.keysym.sym == SDLK_ESCAPE)
            {
              quit = true;
            }
            break;
        }
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

#endif