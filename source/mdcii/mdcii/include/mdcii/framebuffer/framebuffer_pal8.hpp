
// This file is part of the MDCII Game Engine.
// Copyright (C) 2020  Armin Schlegel
// Copyright (C) 2015  Benedikt Freisen
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

#ifndef _FRAMEBUFFER_PAL8_HPP_
#define _FRAMEBUFFER_PAL8_HPP_

#include <inttypes.h>

#include "../bsh/bshreader.hpp"
#include "framebuffer.hpp"

class FramebufferPal8 : public Framebuffer
{
public:
  explicit FramebufferPal8(uint32_t width, uint32_t height, uint32_t color = 0, uint8_t* buffer = NULL, uint32_t bufferLength = 0);
  void DrawBshImage(BshImage& image, int x, int y);
  void DrawPixel(int x, int y, uint8_t color);
  void ExportPNM(const char* path);
  void ExportBMP(const char* path);
  void Clear();

private:
  uint8_t dunkel[256];
  void DrawBshImageFull(BshImage& image, int x, int y);
  void DrawBshImagePartial(BshImage& image, int x, int y);
};

#endif // _FRAMEBUFFER_PAL8_HPP_