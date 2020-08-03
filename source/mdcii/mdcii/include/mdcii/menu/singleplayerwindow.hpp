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
#include <vector>

#include <SDL2/SDL.h>

#include "sdlgui/imageview.h"
#include "sdlgui/screen.h"
#include "sdlgui/texturetable.h"
#include "sdlgui/widget.h"
#include "sdlgui/window.h"

#include "bsh/bshtexture.hpp"
#include "bsh/zeireader.hpp"
#include "bsh/zeitexture.hpp"
#include "cod/basegad_dat.hpp"
#include "cod/buildings.hpp"
#include "cod/cod_parser.hpp"
#include "cod/host_gad.hpp"
#include "files/files.hpp"
#include "framebuffer/palette.hpp"
#include "savegames/savegames.hpp"

#include "menu/fps.hpp"
#include "menu/scale.hpp"

using namespace sdlgui;

class SinglePlayerWindow : public Screen
{
public:
    SinglePlayerWindow(SDL_Renderer* renderer, SDL_Window* pwindow, int width, int height, bool fullscreen);
    void Handle();

private:
    void LoadGame(const std::string& gamName);
    void Redraw();
    SDL_Renderer* renderer;
    int width;
    int height;
    bool fullscreen;
    std::shared_ptr<Buildings> buildings;
    SDL_Window* pwindow;
    Files* files;
    std::shared_ptr<Hostgad> hostgad;
    bool quit;
    StringToSDLTextureConverter stringConverter;
    std::string savegame;
    bool triggerStartGame;
    Scale* scale;
    std::vector<SDL_Texture*> tableStars;
    std::vector<Widget> scenariosList;
    TextureTableBase* scenariosTablePtr;
    TextureTableBase* savegamesTablePtr;
    TextureTableBase* currentTablePtr;
    std::vector<std::tuple<Widget*, int, int>> widgets;
    GamesPb::Games scenes;
};
#endif