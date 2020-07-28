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

#include <inttypes.h>
#include <iostream>
#include <stdlib.h>
#include <string>

#include <boost/program_options.hpp>

#include <SDL2/SDL.h>

#include "camera/bshresources.hpp"
#include "camera/camera.hpp"
#include "cod/cod_parser.hpp"
#include "files/files.hpp"
#include "files/filestocheck.hpp"
#include "framebuffer/framebuffer_pal8.hpp"
#include "framebuffer/palette.hpp"
#include "gam/gam_parser.hpp"
#include "mdcii.hpp"
#include "menu/mainmenu.hpp"
#include "menu/scale.hpp"
#include "spielbildschirm.hpp"
#include "version/version.hpp"

namespace po = boost::program_options;

Uint32 Mdcii::TimerCallback(Uint32 interval, [[maybe_unused]] void* param)
{
  SDL_Event event;
  SDL_UserEvent userevent;

  // push an SDL_USEREVENT event into the queue

  userevent.type = SDL_USEREVENT;
  userevent.code = 0;
  userevent.data1 = NULL;
  userevent.data2 = NULL;

  event.type = SDL_USEREVENT;
  event.user = userevent;

  SDL_PushEvent(&event);
  return (interval);
}


Mdcii::Mdcii(int screen_width, int screen_height, bool fullscreen, const std::string& files_path)
{
  auto files = Files::CreateInstance(files_path);

  Version::DetectGameVersion();
  if (files->CheckAllFiles(&FilesToCheck) == false)
  {
    std::cout << "[ERR] File check failed. Exiting." << std::endl;
    exit(EXIT_FAILURE);
  }

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0)
  {
    exit(EXIT_FAILURE);
  }
  atexit(SDL_Quit);

  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_Window* window = SDL_CreateWindow("mdcii-sdltest", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screen_width, screen_height,
      (fullscreen ? SDL_WINDOW_FULLSCREEN : 0) | SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
  if (window == NULL)
  {
    // In the event that the window could not be made...
    std::cout << "[ERR] Could not create window: " << SDL_GetError() << '\n';
    SDL_Quit();
  }

  SDL_SetWindowMinimumSize(window, 1024, 768);
  Scale::CreateInstance(window);
  SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
  // SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

  Palette::CreateInstance(files->FindPathForFile("stadtfld.col"));
  Buildings::CreateInstance(std::make_shared<CodParser>(files->FindPathForFile("haeuser.cod"), true, false));
  auto basegad = std::make_shared<Basegad>(std::make_shared<CodParser>(files->FindPathForFile("base.gad"), false, false));
  BshResources::CreateInstance();

  MainMenu mainMenu(renderer, basegad, window, screen_width, screen_height, fullscreen);
  mainMenu.Handle();
}
