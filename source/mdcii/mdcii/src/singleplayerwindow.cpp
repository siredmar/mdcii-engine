
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

#include "bsh_texture.hpp"
#include "cod_parser.hpp"
#include "fps.hpp"
#include "palette.hpp"
#include "savegames.hpp"
#include "singleplayerwindow.hpp"

using namespace sdlgui;
SinglePlayerWindow::SinglePlayerWindow(SDL_Renderer* renderer, SDL_Window* pwindow, int rwidth, int rheight, bool fullscreen)
  : renderer(renderer)
  , width(rwidth)
  , height(rheight)
  , fullscreen(fullscreen)
  , pwindow(pwindow)
  , files(Files::instance())
  , hostgad(std::make_shared<Hostgad>(std::make_shared<Cod_Parser>(files->instance()->find_path_for_file("host.gad"), false, false)))
  , quit(false)
  , Screen(pwindow, Vector2i(rwidth, rheight), "Game", false, true)
{
  std::cout << "host.gad: " << hostgad->get_gadgets_size() << std::endl;
  Bsh_leser bsh_leser(files->instance()->find_path_for_file("toolgfx/start.bsh"));
  BshImageToSDLTextureConverter converter(renderer);

  SDL_Texture* background = converter.Convert(&bsh_leser.gib_bsh_bild(0));

  auto tableGad = hostgad->get_gadgets_by_index(1);
  SDL_Texture* tableTexture = converter.Convert(&bsh_leser.gib_bsh_bild(tableGad->Gfxnr));

  auto newGameButtonGad = hostgad->get_gadgets_by_index(2);
  SDL_Texture* newGameTexture = converter.Convert(&bsh_leser.gib_bsh_bild(newGameButtonGad->Gfxnr));
  SDL_Texture* newGameTextureClicked = converter.Convert(&bsh_leser.gib_bsh_bild(newGameButtonGad->Gfxnr + newGameButtonGad->Pressoff));

  auto loadGameButtonGad = hostgad->get_gadgets_by_index(3);
  SDL_Texture* loadGameTexture = converter.Convert(&bsh_leser.gib_bsh_bild(loadGameButtonGad->Gfxnr));
  SDL_Texture* loadGameTextureClicked = converter.Convert(&bsh_leser.gib_bsh_bild(loadGameButtonGad->Gfxnr + loadGameButtonGad->Pressoff));

  auto continueGameButtonGad = hostgad->get_gadgets_by_index(4);
  SDL_Texture* continueTexture = converter.Convert(&bsh_leser.gib_bsh_bild(continueGameButtonGad->Gfxnr));
  SDL_Texture* continueTextureClicked = converter.Convert(&bsh_leser.gib_bsh_bild(continueGameButtonGad->Gfxnr + continueGameButtonGad->Pressoff));

  auto mainMenuButtonGad = hostgad->get_gadgets_by_index(5);
  SDL_Texture* mainMenuTexture = converter.Convert(&bsh_leser.gib_bsh_bild(mainMenuButtonGad->Gfxnr));
  SDL_Texture* mainMenuTextureClicked = converter.Convert(&bsh_leser.gib_bsh_bild(mainMenuButtonGad->Gfxnr + mainMenuButtonGad->Pressoff));

  {
    wdg<TextureView>(background);
    auto& table = wdg<TextureView>(tableTexture);
    table.setPosition(tableGad->Pos.x, tableGad->Pos.y);

    auto& newGameButton = wdg<TextureButton>(newGameTexture, [this] {
      std::cout << "Singleplayer pressed" << std::endl;
      // triggerStartGame = true;
    });
    newGameButton.setPosition(newGameButtonGad->Pos.x, newGameButtonGad->Pos.y);
    newGameButton.setSecondaryTexture(newGameTextureClicked);
    newGameButton.setTextureSwitchFlags(TextureButton::OnClick);

    auto& loadGameButton = wdg<TextureButton>(loadGameTexture, [this] { std::cout << "Load Game pressed" << std::endl; });
    loadGameButton.setPosition(loadGameButtonGad->Pos.x, loadGameButtonGad->Pos.y);
    loadGameButton.setSecondaryTexture(loadGameTextureClicked);
    loadGameButton.setTextureSwitchFlags(TextureButton::OnClick);

    auto& continueGameButton = wdg<TextureButton>(continueTexture, [this] { std::cout << "Continue Game pressed" << std::endl; });
    continueGameButton.setPosition(continueGameButtonGad->Pos.x, continueGameButtonGad->Pos.y);
    continueGameButton.setSecondaryTexture(continueTextureClicked);
    continueGameButton.setTextureSwitchFlags(TextureButton::OnClick);

    auto& mainMenuButton = wdg<TextureButton>(mainMenuTexture, [this] {
      std::cout << "Main Menu pressed" << std::endl;
      quit = true;
    });
    mainMenuButton.setPosition(mainMenuButtonGad->Pos.x, mainMenuButtonGad->Pos.y);
    mainMenuButton.setSecondaryTexture(mainMenuTextureClicked);
    mainMenuButton.setTextureSwitchFlags(TextureButton::OnClick);

    Savegames save;
    auto s = save.getSavegames();

    for (int i = 0; i < s.size(); i++)
    {
      std::cout << "Savegame " << i << ": " << s[i] << std::endl;
    }
  }
  performLayout(mSDL_Renderer);
}

// void MainMenu::LoadGame(const std::string& gam_name)
// {
//   auto haeuser_cod = std::make_shared<Cod_Parser>(files->instance()->find_path_for_file("haeuser.cod"), true, false);
//   auto haeuser = std::make_shared<Haeuser>(haeuser_cod);

//   GameWindow gameWindow(renderer, haeuser, pwindow, width, height, gam_name, fullscreen);
//   gameWindow.Handle();
//   Handle();
// }

// todo: add signal/slot for exiting window
void SinglePlayerWindow::Handle()
{
  auto palette = Palette::instance();

  auto transparentColor = palette->getColor(palette->getTransparentColor());
  std::cout << "[INFO] Transparent color: " << (int)transparentColor.getRed() << ", " << (int)transparentColor.getGreen() << ", "
            << (int)transparentColor.getBlue() << std::endl;

  SDL_Texture* texture;
  SDL_Surface* final_surface;

  SDL_Surface* s8 = SDL_CreateRGBSurface(0, width, height, 8, 0, 0, 0, 0);
  SDL_SetPaletteColors(s8->format->palette, palette->getSDLColors(), 0, palette->size());

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
      SDL_FreeSurface(final_surface);
      SDL_RenderClear(renderer);
      SDL_RenderCopy(renderer, texture, NULL, NULL);
      SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_NONE);
      // if (triggerStartGame)
      // {
      //   triggerStartGame = false;
      //   this->LoadGame(this->gam_name);
      // }
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
