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

#ifndef GAMEWINDOW_H_
#define GAMEWINDOW_H_

#include <iostream>
#include <memory>

#include <SDL2/SDL.h>

#include "sdlgui/imageview.h"
#include "sdlgui/screen.h"
#include "sdlgui/window.h"

#include "cod/cod_parser.hpp"
#include "cod/haeuser.hpp"
#include "files.hpp"

using namespace sdlgui;

class GameWindow : public Screen
{
public:
  GameWindow(
      SDL_Renderer* renderer, SDL_Window* pwindow, int rwidth, int rheight, const std::string& gam_name, bool fullscreen, std::shared_ptr<Haeuser> haeuser);
  void Handle();

private:
  SDL_Renderer* renderer;
  int width;
  int height;
  std::string gam_name;
  bool fullscreen;
  std::shared_ptr<Haeuser> haeuser;
  bool running;
};
#endif