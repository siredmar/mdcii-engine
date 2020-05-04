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

#include <codecvt>
#include <ios>
#include <iostream>
#include <locale>

#include "bildspeicher_trans_pal8.hpp"
#include "zei_leser.hpp"
#include "zei_texture.hpp"

StringToSDLTextureConverter::StringToSDLTextureConverter(SDL_Renderer* renderer)
  : renderer(renderer)
  , files(Files::instance())
  , zei(std::make_shared<Zei_leser>(files->instance()->find_path_for_file("zei16v.zei")))
{
}

SDL_Texture* StringToSDLTextureConverter::Convert(const std::string& str, int color, int shadowColor, int verticalMargin)
{
  auto palette = Palette::instance();
  int transparent = palette->getTransparentColor();
  auto transparentColor = palette->getColor(transparent);
  int stringLength = 0;
  int stringHeight = 0;

  std::wstring wide = converter.from_bytes(str);
  for (auto& ch : wide)
  {
    try
    {
      Zei_zeichen& zz = zei->gib_bsh_bild(ch - ' ');
      stringLength += zz.breite;
      if (stringHeight < zz.hoehe)
      {
        stringHeight = zz.hoehe;
        if (verticalMargin > 0)
        {
          stringHeight += verticalMargin * 2;
        }
      }
    }
    catch (std::exception& ex)
    {
      std::cout << ex.what() << std::endl;
    }
  }

  SDL_Surface* final_surface;
  SDL_Surface* s8 = SDL_CreateRGBSurface(0, stringLength, stringHeight, 8, 0, 0, 0, 0);
  SDL_SetPaletteColors(s8->format->palette, palette->getSDLColors(), 0, palette->size());
  Bildspeicher_trans_pal8 bs(
      stringLength, stringHeight, palette->getTransparentColor(), static_cast<uint8_t*>(s8->pixels), (uint32_t)s8->pitch, palette->getTransparentColor());
  bs.setze_schriftfarbe(color, shadowColor);
  bs.zeichne_string(*zei, wide, 0, verticalMargin);
  final_surface = SDL_ConvertSurfaceFormat(s8, SDL_PIXELFORMAT_RGB888, 0);
  SDL_SetColorKey(
      final_surface, SDL_TRUE, SDL_MapRGB(final_surface->format, transparentColor.getRed(), transparentColor.getGreen(), transparentColor.getBlue()));
  auto texture = SDL_CreateTextureFromSurface(renderer, final_surface);
  return texture;
}