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

#ifndef SINGLEPLAYERWINDOW_H_
#define SINGLEPLAYERWINDOW_H_

#include <iostream>
#include <memory>

#include <SDL2/SDL.h>

#include "sdlgui/imageview.h"
#include "sdlgui/screen.h"
#include "sdlgui/widget.h"
#include "sdlgui/window.h"

#include "basegad_dat.hpp"
#include "files.hpp"
#include "haeuser.hpp"
#include "host_gad.hpp"
#include "zei_leser.hpp"
#include "zei_texture.hpp"

using namespace sdlgui;

class SinglePlayerWindow : public Screen
{
public:
  SinglePlayerWindow(SDL_Renderer* renderer, SDL_Window* pwindow, int rwidth, int rheight, bool fullscreen, std::shared_ptr<Haeuser> haeuser);
  void Handle();

private:
  void LoadGame(const std::string& gam_name);
  Widget& ListTable(Widget* parent, const std::vector<std::tuple<std::string, std::string, int>>& list, int x, int y, int verticalMargin);

  SDL_Renderer* renderer;
  int width;
  int height;
  bool fullscreen;
  std::shared_ptr<Haeuser> haeuser;
  SDL_Window* pwindow;
  Files* files;
  bool quit;
  std::shared_ptr<Hostgad> hostgad;
  StringToSDLTextureConverter stringConverter;
  std::string savegame;
  bool triggerStartGame;
  std::vector<SDL_Texture*> tableStars;
};
#endif