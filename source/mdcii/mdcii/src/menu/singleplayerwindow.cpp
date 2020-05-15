
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

#include "sdlgui/label.h"
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
#include "zei_texture.hpp"

#include "menu/gamewindow.hpp"
#include "menu/singleplayerwindow.hpp"

using namespace sdlgui;
SinglePlayerWindow::SinglePlayerWindow(SDL_Renderer* renderer, SDL_Window* pwindow, int rwidth, int rheight, bool fullscreen, std::shared_ptr<Haeuser> haeuser)
  : Screen(pwindow, Vector2i(rwidth, rheight), "Game", false, true)
  , renderer(renderer)
  , width(rwidth)
  , height(rheight)
  , fullscreen(fullscreen)
  , haeuser(haeuser)
  , pwindow(pwindow)
  , files(Files::instance())
  , hostgad(std::make_shared<Hostgad>(std::make_shared<Cod_Parser>(files->instance()->find_path_for_file("host.gad"), false, false)))
  , quit(false)
  , stringConverter(StringToSDLTextureConverter(renderer, "zei20v.zei"))
  , savegame("")
  , triggerStartGame(false)
{
  std::cout << "host.gad: " << hostgad->get_gadgets_size() << std::endl;
  Bsh_leser bsh_leser(files->instance()->find_path_for_file("toolgfx/start.bsh"));
  BshImageToSDLTextureConverter converter(renderer);

  SDL_Texture* background = converter.Convert(&bsh_leser.gib_bsh_bild(0));

  auto tableGad = hostgad->get_gadgets_by_index(1);
  SDL_Texture* tableTexture = converter.Convert(&bsh_leser.gib_bsh_bild(tableGad->Gfxnr));

  for (int i = 0; i < 5; i++)
  {
    tableStars.push_back(converter.Convert(&bsh_leser.gib_bsh_bild(21 + i)));
  }

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
    // background
    wdg<TextureView>(background);
    auto& tableFrame = wdg<TextureView>(tableTexture);
    tableFrame.setPosition(tableGad->Pos.x, tableGad->Pos.y);

    // table
    /// savegames
    Savegames saveRaw("/savegame", ".gam");
    auto saves = saveRaw.getSavegames();
    for (unsigned int i = 0; i < saves.size(); i++)
    {
      std::cout << "Savegame " << i << ": " << std::get<1>(saves[i]) << std::endl;
    }
    auto& savegameTable = ListTable(this, saves, tableGad->Pos.x + 20, tableGad->Pos.y + 13, 2);
    savegameTable.setVisible(false);

    /// scenes
    Savegames szenesRaw("/szenes", ".szs");
    auto szenes = szenesRaw.getSavegames();

    for (unsigned int i = 0; i < szenes.size(); i++)
    {
      std::cout << "Szenes " << i << ": " << std::get<1>(szenes[i]) << std::endl;
    }
    auto& szenesTable = ListTable(this, szenes, tableGad->Pos.x + 20, tableGad->Pos.y + 13, 2);
    szenesTable.setVisible(true);

    // buttons
    auto& newGameButton = wdg<TextureButton>(newGameTexture, [this, &savegameTable, &szenesTable] {
      std::cout << "Singleplayer pressed" << std::endl;
      savegameTable.setVisible(false);
      szenesTable.setVisible(true);
    });
    newGameButton.setPosition(newGameButtonGad->Pos.x, newGameButtonGad->Pos.y);
    newGameButton.setSecondaryTexture(newGameTextureClicked);
    newGameButton.setTextureSwitchFlags(TextureButton::OnClick);

    auto& loadGameButton = wdg<TextureButton>(loadGameTexture, [this, &savegameTable, &szenesTable] {
      std::cout << "Load Game pressed" << std::endl;
      savegameTable.setVisible(true);
      szenesTable.setVisible(false);
    });
    loadGameButton.setPosition(loadGameButtonGad->Pos.x, loadGameButtonGad->Pos.y);
    loadGameButton.setSecondaryTexture(loadGameTextureClicked);
    loadGameButton.setTextureSwitchFlags(TextureButton::OnClick);

    auto& continueGameButton = wdg<TextureButton>(continueTexture, [this] {
      std::cout << "Continue Game pressed" << std::endl;
      LoadGame(Files::instance()->find_path_for_file("lastgame.gam"));
    });
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
  }
  performLayout(mSDL_Renderer);
}

Widget& SinglePlayerWindow::ListTable(
    [[maybe_unused]] Widget* parent, const std::vector<std::tuple<std::string, std::string, int>>& list, int x, int y, [[maybe_unused]] int verticalMargin)
{
  auto& table = this->widget().boxlayout(Orientation::Vertical, Alignment::Minimum, 0, 1);
  table.setPosition(x, y);
  int index = 0;
  for (auto& entry : list)
  {
    auto& tableEntry = table.widget();
    auto texture = stringConverter.Convert(std::get<1>(entry), 243, 0, 0);
    auto textureHover = stringConverter.Convert(std::get<1>(entry), 245, 0, 0);
    auto& button = tableEntry.texturebutton(texture, [this, entry] {
      std::cout << std::get<1>(entry) << " clicked" << std::endl;
      savegame = std::get<0>(entry);
      triggerStartGame = true;
    });
    auto size = button.size();
    tableEntry.setSize(size);
    if (std::get<2>(entry) >= 0)
    {
      auto& star = tableEntry.textureview(tableStars.at(std::get<2>(entry)));
      star.setPosition(320, 0);
    }
    button.setSecondaryTexture(textureHover);
    button.setTextureSwitchFlags(TextureButton::OnClick);
    index++;
    // TODO: Add up and down handler buttons. Until this is done -> bail out at 10 entries
    if (index >= 10)
    {
      break;
    }
  }
  return table;
}

void SinglePlayerWindow::LoadGame(const std::string& gam_name)
{
  if (files->instance()->check_file(gam_name) == false)
  {
    std::cout << "[ERR] Could not load savegame: " << gam_name << std::endl;
    exit(EXIT_FAILURE);
  }

  GameWindow gameWindow(renderer, pwindow, width, height, gam_name, fullscreen, haeuser);
  gameWindow.Handle();
  Handle();
}

// todo: add signal/slot for exiting window
void SinglePlayerWindow::Handle()
{
  auto palette = Palette::instance();

  auto transparentColor = palette->getColor(palette->getTransparentColor());
  std::cout << "[INFO] Transparent color: " << (int)transparentColor.getRed() << ", " << (int)transparentColor.getGreen() << ", "
            << (int)transparentColor.getBlue() << std::endl;

  auto s8 = SDL_CreateRGBSurface(0, width, height, 8, 0, 0, 0, 0);
  SDL_SetPaletteColors(s8->format->palette, palette->getSDLColors(), 0, palette->size());

  try
  {
    auto final_surface = SDL_ConvertSurfaceFormat(s8, SDL_PIXELFORMAT_RGB888, 0);
    auto texture = SDL_CreateTextureFromSurface(renderer, final_surface);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    this->drawAll();
    SDL_RenderPresent(renderer);

    Fps fps;
    // later used for key inputs, uncommented for now
    // const Uint8* keystate = SDL_GetKeyboardState(NULL);

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

      final_surface = SDL_ConvertSurfaceFormat(s8, SDL_PIXELFORMAT_RGB888, 0);
      texture = SDL_CreateTextureFromSurface(renderer, final_surface);
      SDL_FreeSurface(final_surface);
      SDL_RenderClear(renderer);
      SDL_RenderCopy(renderer, texture, NULL, NULL);
      SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_NONE);
      if (triggerStartGame)
      {
        triggerStartGame = false;
        this->LoadGame(savegame);
      }
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
