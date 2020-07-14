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

#include <ios>
#include <iostream>

#include "bsh/bshtexture.hpp"
#include "bsh/zeireader.hpp"
#include "framebuffer/framebuffer_trans_pal8.hpp"

BshImageToSDLTextureConverter::BshImageToSDLTextureConverter(SDL_Renderer* renderer)
  : renderer(renderer)
{
}

SDL_Texture* BshImageToSDLTextureConverter::Convert(BshImage* image)
{
  auto palette = Palette::Instance();
  SDL_Surface* finalSurface;
  SDL_Surface* s8 = SDL_CreateRGBSurface(0, image->width, image->height, 8, 0, 0, 0, 0);
  SDL_SetPaletteColors(s8->format->palette, palette->GetSDLColors(), 0, palette->size());
  FramebufferTransparentPal8 fb(image->width, image->height, 0, static_cast<uint8_t*>(s8->pixels), (uint32_t)s8->pitch, palette->GetTransparentColor());
  fb.DrawBshImage(*image, 0, 0);
  auto transparentColor = palette->GetColor(palette->GetTransparentColor());
  finalSurface = SDL_ConvertSurfaceFormat(s8, SDL_PIXELFORMAT_RGB888, 0);
  SDL_SetColorKey(finalSurface, SDL_TRUE, SDL_MapRGB(finalSurface->format, transparentColor.getRed(), transparentColor.getGreen(), transparentColor.getBlue()));
  auto texture = SDL_CreateTextureFromSurface(renderer, finalSurface);
  return texture;
}