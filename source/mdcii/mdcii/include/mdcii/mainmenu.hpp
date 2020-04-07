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

#ifndef MAINMENU_H_
#define MAINMENU_H_

#include <iostream>
#include <memory>

#include <SDL2/SDL.h>

#include "sdlgui/imageview.h"
#include "sdlgui/screen.h"
#include "sdlgui/window.h"

#include "basegad_dat.hpp"
#include "cod_parser.hpp"
#include "files.hpp"


using namespace sdlgui;

class MainMenu : public Screen
{
public:
  MainMenu(
      SDL_Renderer* renderer, std::shared_ptr<Basegad> basegad, SDL_Window* pwindow, int rwidth, int rheight, bool fullscreen, const std::string& gam_name);
  void Handle();
  void LoadGame(const std::string& gam_name);

private:
  SDL_Renderer* renderer;
  std::shared_ptr<Basegad> basegad;
  int width;
  int height;
  bool fullscreen;
  std::string gam_name;
  bool triggerStartGame;
  SDL_Window* pwindow;
  Files* files;
};
#endif