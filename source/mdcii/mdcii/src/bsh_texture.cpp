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

#include "bildspeicher_trans_pal8.hpp"
#include "bsh_texture.hpp"
#include "zei_leser.hpp"

BshImageToSDLTextureConverter::BshImageToSDLTextureConverter(SDL_Renderer* renderer)
  : renderer(renderer)
{
}

SDL_Texture* BshImageToSDLTextureConverter::Convert(Bsh_bild* image)
{
  auto palette = Palette::instance();
  SDL_Surface* final_surface;
  SDL_Surface* s8 = SDL_CreateRGBSurface(0, image->breite, image->hoehe, 8, 0, 0, 0, 0);
  SDL_SetPaletteColors(s8->format->palette, palette->getSDLColors(), 0, palette->size());
  Bildspeicher_trans_pal8 bs(image->breite, image->hoehe, 0, static_cast<uint8_t*>(s8->pixels), (uint32_t)s8->pitch, palette->getTransparentColor());
  bs.zeichne_bsh_bild(*image, 0, 0);
  auto transparentColor = palette->getColor(palette->getTransparentColor());
  final_surface = SDL_ConvertSurfaceFormat(s8, SDL_PIXELFORMAT_RGB888, 0);
  SDL_SetColorKey(
      final_surface, SDL_TRUE, SDL_MapRGB(final_surface->format, transparentColor.getRed(), transparentColor.getGreen(), transparentColor.getBlue()));
  auto texture = SDL_CreateTextureFromSurface(renderer, final_surface);
  return texture;
}