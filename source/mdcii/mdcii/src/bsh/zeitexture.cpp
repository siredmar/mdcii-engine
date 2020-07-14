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

#include "bsh/zeireader.hpp"
#include "bsh/zeitexture.hpp"
#include "framebuffer/framebuffer_trans_pal8.hpp"

StringToSDLTextureConverter::StringToSDLTextureConverter(SDL_Renderer* renderer, const std::string& font)
  : renderer(renderer)
  , font(font)
  , files(Files::Instance())
  , zei(std::make_shared<ZeiReader>(files->Instance()->FindPathForFile(font)))
{
}

SDL_Texture* StringToSDLTextureConverter::Convert(const std::string& str, int color, int shadowColor, int verticalMargin)
{
  auto palette = Palette::Instance();
  int transparent = palette->GetTransparentColor();
  auto transparentColor = palette->GetColor(transparent);
  unsigned int stringLength = 0;
  unsigned int stringHeight = 0;

  std::wstring wide = converter.from_bytes(str);
  for (auto& ch : wide)
  {
    try
    {
      ZeiCharacter& zz = zei->GetBshImage(ch - ' ');
      stringLength += zz.width;
      if (stringHeight < zz.height)
      {
        stringHeight = zz.height;
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

  SDL_Surface* finalSurface;
  SDL_Surface* s8 = SDL_CreateRGBSurface(0, static_cast<int>(stringLength), static_cast<int>(stringHeight), 8, 0, 0, 0, 0);
  SDL_SetPaletteColors(s8->format->palette, palette->GetSDLColors(), 0, palette->size());
  FramebufferTransparentPal8 fb(
      stringLength, stringHeight, palette->GetTransparentColor(), static_cast<uint8_t*>(s8->pixels), (uint32_t)s8->pitch, palette->GetTransparentColor());
  fb.SetFontColor(color, shadowColor);
  fb.DrawString(*zei, wide, 0, verticalMargin);
  finalSurface = SDL_ConvertSurfaceFormat(s8, SDL_PIXELFORMAT_RGB888, 0);
  SDL_SetColorKey(finalSurface, SDL_TRUE, SDL_MapRGB(finalSurface->format, transparentColor.getRed(), transparentColor.getGreen(), transparentColor.getBlue()));
  auto texture = SDL_CreateTextureFromSurface(renderer, finalSurface);
  return texture;
}