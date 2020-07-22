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

// Based on nanogui-sdl by Dalerank <dalerankn8@gmail.com>
#ifndef _SCALE_HPP_
#define _SCALE_HPP_

#include <SDL2/SDL.h>

class Scale
{
public:
  struct ScreenSize
  {
    ScreenSize(uint32_t width, uint32_t height)
      : width(width)
      , height(height)
    {
    }
    uint32_t width;
    uint32_t height;
  };

  static Scale* CreateInstance(SDL_Window* window);
  static Scale* Instance();
  ScreenSize GetScreenSize();
  int GetScreenWidth();
  int GetScreenHeight();
  void SetScreenSize(ScreenSize newSize);
  void SetFullscreen(bool enabled);
  void ToggleFullscreen();

private:
  bool fullscreen;
  SDL_Window* window;
  static Scale* _instance;
  Scale(SDL_Window* window);
  class CGuard
  {
  public:
    ~CGuard()
    {
      if (NULL != Scale::_instance)
      {
        delete Scale::_instance;
        Scale::_instance = NULL;
      }
    }
  };
};

#endif // _SCALE_HPP_