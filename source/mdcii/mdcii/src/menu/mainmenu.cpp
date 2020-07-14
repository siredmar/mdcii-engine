
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

#include "sdlgui/layout.h"
#include "sdlgui/messagedialog.h"
#include "sdlgui/screen.h"
#include "sdlgui/texturebutton.h"
#include "sdlgui/textureview.h"
#include "sdlgui/window.h"

#include "menu/mainmenu.hpp"
#include "menu/singleplayerwindow.hpp"

using namespace sdlgui;
MainMenu::MainMenu(
    SDL_Renderer* renderer, std::shared_ptr<Buildings> buildings, std::shared_ptr<Basegad> basegad, SDL_Window* pwindow, int width, int height, bool fullscreen)
  : Screen(pwindow, Vector2i(width, height), "Game", false, true)
  , renderer(renderer)
  , buildings(buildings)
  , basegad(basegad)
  , width(width)
  , height(height)
  , fullscreen(fullscreen)
  , triggerSinglePlayer(false)
  , pwindow(pwindow)
  , files(Files::Instance())
  , quit(false)
{
  std::cout << "Basegad: " << basegad->GetGadgetsSize() << std::endl;
  BshReader bsh_leser(files->Instance()->FindPathForFile("toolgfx/start.bsh"));
  BshImageToSDLTextureConverter converter(renderer);

  SDL_Texture* background = converter.Convert(&bsh_leser.GetBshImage(0));

  auto shipGad = basegad->GetGadgetsByIndex(1);
  SDL_Texture* shipTexture = converter.Convert(&bsh_leser.GetBshImage(shipGad->Gfxnr));

  auto singlePlayerButtonGad = basegad->GetGadgetsByIndex(2);
  SDL_Texture* singlePlayerTexture = converter.Convert(&bsh_leser.GetBshImage(singlePlayerButtonGad->Gfxnr));
  SDL_Texture* singlePlayerTextureClicked = converter.Convert(&bsh_leser.GetBshImage(singlePlayerButtonGad->Gfxnr + singlePlayerButtonGad->Pressoff));

  auto multiPlayerButtonGad = basegad->GetGadgetsByIndex(3);
  SDL_Texture* multiPlayerTexture = converter.Convert(&bsh_leser.GetBshImage(multiPlayerButtonGad->Gfxnr));
  SDL_Texture* multiPlayerTextureClicked = converter.Convert(&bsh_leser.GetBshImage(multiPlayerButtonGad->Gfxnr + multiPlayerButtonGad->Pressoff));

  auto creditsButtonGad = basegad->GetGadgetsByIndex(4);
  SDL_Texture* creditsTexture = converter.Convert(&bsh_leser.GetBshImage(creditsButtonGad->Gfxnr));
  SDL_Texture* creditsTextureClicked = converter.Convert(&bsh_leser.GetBshImage(creditsButtonGad->Gfxnr + creditsButtonGad->Pressoff));

  auto introButtonGad = basegad->GetGadgetsByIndex(5);
  SDL_Texture* introTexture = converter.Convert(&bsh_leser.GetBshImage(introButtonGad->Gfxnr));
  SDL_Texture* introTextureClicked = converter.Convert(&bsh_leser.GetBshImage(introButtonGad->Gfxnr + introButtonGad->Pressoff));

  auto exitButtonGad = basegad->GetGadgetsByIndex(6);
  SDL_Texture* exitTexture = converter.Convert(&bsh_leser.GetBshImage(exitButtonGad->Gfxnr));
  SDL_Texture* exitTextureClicked = converter.Convert(&bsh_leser.GetBshImage(exitButtonGad->Gfxnr + exitButtonGad->Pressoff));

  {
    wdg<TextureView>(background);
    auto& ship = wdg<TextureView>(shipTexture);
    ship.setPosition(shipGad->Pos.x, shipGad->Pos.y);

    auto& singlePlayerButton = wdg<TextureButton>(singlePlayerTexture, [this] {
      std::cout << "Singleplayer pressed" << std::endl;
      triggerSinglePlayer = true;
    });
    singlePlayerButton.setPosition(singlePlayerButtonGad->Pos.x, singlePlayerButtonGad->Pos.y);
    singlePlayerButton.setSecondaryTexture(singlePlayerTextureClicked);
    singlePlayerButton.setTextureSwitchFlags(TextureButton::OnClick);

    auto& multiPlayerButton = wdg<TextureButton>(multiPlayerTexture, [this] { std::cout << "Multiplayer pressed" << std::endl; });
    multiPlayerButton.setPosition(multiPlayerButtonGad->Pos.x, multiPlayerButtonGad->Pos.y);
    multiPlayerButton.setSecondaryTexture(multiPlayerTextureClicked);
    multiPlayerButton.setTextureSwitchFlags(TextureButton::OnClick);

    auto& creditsButton = wdg<TextureButton>(creditsTexture, [this] { std::cout << "credits pressed" << std::endl; });
    creditsButton.setPosition(creditsButtonGad->Pos.x, creditsButtonGad->Pos.y);
    creditsButton.setSecondaryTexture(creditsTextureClicked);
    creditsButton.setTextureSwitchFlags(TextureButton::OnClick);

    auto& introButton = wdg<TextureButton>(introTexture, [this] { std::cout << "intro pressed" << std::endl; });
    introButton.setPosition(introButtonGad->Pos.x, introButtonGad->Pos.y);
    introButton.setSecondaryTexture(introTextureClicked);
    introButton.setTextureSwitchFlags(TextureButton::OnClick);

    auto& exitButton = wdg<TextureButton>(exitTexture, [this] {
      std::cout << "exit pressed" << std::endl;
      quit = true;
    });
    exitButton.setPosition(exitButtonGad->Pos.x, exitButtonGad->Pos.y);
    exitButton.setSecondaryTexture(exitTextureClicked);
    exitButton.setTextureSwitchFlags(TextureButton::OnClick);
  }
  performLayout(renderer);
}

// todo: add signal/slot for exiting window
void MainMenu::Handle()
{
  auto palette = Palette::Instance();

  auto transparentColor = palette->GetColor(palette->GetTransparentColor());
  std::cout << "[INFO] Transparent color: " << (int)transparentColor.getRed() << ", " << (int)transparentColor.getGreen() << ", "
            << (int)transparentColor.getBlue() << std::endl;

  auto s8 = SDL_CreateRGBSurface(0, width, height, 8, 0, 0, 0, 0);
  SDL_SetPaletteColors(s8->format->palette, palette->GetSDLColors(), 0, palette->size());

  try
  {
    auto finalSurface = SDL_ConvertSurfaceFormat(s8, SDL_PIXELFORMAT_RGB888, 0);
    auto texture = SDL_CreateTextureFromSurface(renderer, finalSurface);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    this->drawAll();
    SDL_RenderPresent(renderer);

    Fps fps;

    SinglePlayerWindow singleplayerwindow(renderer, pwindow, width, height, fullscreen, buildings);
    SDL_Event e;
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
          default:
            break;
        }
        this->onEvent(e);
      }
      int x, y;
      SDL_GetMouseState(&x, &y);

      finalSurface = SDL_ConvertSurfaceFormat(s8, SDL_PIXELFORMAT_RGB888, 0);
      texture = SDL_CreateTextureFromSurface(renderer, finalSurface);
      SDL_FreeSurface(finalSurface);
      SDL_RenderClear(renderer);
      SDL_RenderCopy(renderer, texture, NULL, NULL);
      SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_NONE);
      if (triggerSinglePlayer)
      {
        triggerSinglePlayer = false;
        singleplayerwindow.Handle();
        Handle();
      }
      this->drawAll();
      SDL_RenderPresent(renderer);
      SDL_DestroyTexture(texture);
      fps.Next();
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
