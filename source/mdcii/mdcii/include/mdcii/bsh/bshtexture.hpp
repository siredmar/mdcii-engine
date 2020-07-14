// This file is part of the MDCII Game Engine.
// Copyright (C) 2020  Armin Schlegel
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

#ifndef _BSH_TEXTURE_HPP
#define _BSH_TEXTURE_HPP

#include <SDL2/SDL.h>

#include "bshreader.hpp"
#include "framebuffer/framebuffer_pal8.hpp"
#include "framebuffer/palette.hpp"

class BshImageToSDLTextureConverter
{
public:
  explicit BshImageToSDLTextureConverter(SDL_Renderer* renderer);
  SDL_Texture* Convert(BshImage* image);

private:
  SDL_Renderer* renderer;
};

#endif // _BSH_TEXTURE_HPP