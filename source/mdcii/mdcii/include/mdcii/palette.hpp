// This file is part of the MDCII Game Engine.
// Copyright (C) 2019  Armin Schlegel
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

#ifndef _PALETTE_HPP
#define _PALETTE_HPP

#include <vector>

#include <SDL2/SDL.h>


class PaletteColor
{
public:
  PaletteColor(uint8_t r, uint8_t g, uint8_t b)
    : r(r)
    , g(g)
    , b(b)
  {
  }

  uint8_t getRed() { return r; }
  uint8_t getGreen() { return g; }
  uint8_t getBlue() { return b; }

  bool operator==(PaletteColor const& obj)
  {
    if ((r == obj.r) && (g == obj.g) && (b == obj.b))
    {
      return true;
    }
    return false;
  }


private:
  uint8_t r;
  uint8_t g;
  uint8_t b;
};

class Palette
{
public:
  static Palette* create_instance(const std::string& palette_file_path);
  static Palette* instance();
  void init(const std::string& palette_file_path);

  std::vector<PaletteColor> getPalette();
  SDL_Color* getSDLColors();
  int size();
  int getTransparentColor();
  PaletteColor getColor(int index);
  uint8_t index(int index);

private:
  std::string path;
  std::vector<PaletteColor> palette;
  int transparentColor;
  SDL_Color* c;

  static Palette* _instance;
  ~Palette() {}
  Palette(const std::string& palette_file_path) { init(palette_file_path); }

  Palette(const Palette&);

  int findTransparentColorIndex();

  class CGuard
  {
  public:
    ~CGuard()
    {
      if (NULL != Palette::_instance)
      {
        delete Palette::_instance;
        Palette::_instance = NULL;
      }
    }
  };
};

#endif