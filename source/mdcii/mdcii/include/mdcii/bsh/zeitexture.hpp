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

#ifndef _ZEI_TEXTURE
#define _ZEI_TEXTURE

#include <codecvt>
#include <locale>

#include <SDL2/SDL.h>

#include "bshreader.hpp"
#include "files/files.hpp"
#include "framebuffer/framebuffer_pal8.hpp"
#include "framebuffer/palette.hpp"

class StringToSDLTextureConverter
{
public:
  explicit StringToSDLTextureConverter(SDL_Renderer* renderer, const std::string& font);
  SDL_Texture* Convert(const std::string& str, int color = 245, int shadowColor = 0, int verticalMargin = 0);

private:
  SDL_Renderer* renderer;
  std::string font;
  Files* files;
  std::shared_ptr<ZeiReader> zei;
  std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
};

#endif