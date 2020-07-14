
// This file is part of the MDCII Game Engine.
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

#include "framebuffer/framebuffer_trans_pal8.hpp"
#include "framebuffer/palette.hpp"
#include <fstream>
#include <ios>
#include <iostream>
#include <string.h>

FramebufferTransparentPal8::FramebufferTransparentPal8(
    uint32_t width, uint32_t height, uint32_t color, uint8_t* buffer, uint32_t bufferLength, uint8_t transparent)
  : Framebuffer(width, height, 1, color, buffer, bufferLength)
  , transparent(transparent)
{
  Clear();
}

void FramebufferTransparentPal8::DrawBshImageFull(BshImage& image, int x, int y)
{
  uint8_t* quelle = image.buffer;

  // Read until we reach the end marker
  while (true)
  {
    int numAlpha = *(quelle++);

    // End marker
    if (numAlpha == 255)
    {
      int rest = this->bufferLength - x;
      for (int i = x; i < x + rest; i++)
      {
        buffer[y * bufferLength + i] = transparent;
      }
      break;
    }

    // End of row
    if (numAlpha == 254)
    {
      int rest = this->bufferLength - x;
      for (int i = x; i < x + rest; i++)
      {
        buffer[y * bufferLength + i] = transparent;
      }
      x = 0;
      y++;
      continue;
    }

    // Pixel data
    for (int i = 0; i < numAlpha; i++)
    {
      buffer[y * bufferLength + x] = transparent;
      x++;
    }
    int numPixels = *(quelle++);
    for (int i = 0; i < numPixels; i++)
    {
      int colourIndex = *(quelle++);
      buffer[y * bufferLength + x] = colourIndex;
      x++;
    }
  }
}

void FramebufferTransparentPal8::DrawBshImage(BshImage& image, int x, int y)
{
  if (x >= (int)this->width || y >= (int)this->height || x + (int)image.width < 0 || y + (int)image.height < 0)
  {
    return;
  }
  DrawBshImageFull(image, x, y);
}

void FramebufferTransparentPal8::DrawPixel(int x, int y, uint8_t color)
{
  if (x < 0 || y < 0 || x >= static_cast<int>(width) || y >= static_cast<int>(height))
  {
    return;
  }
  buffer[y * bufferLength + x] = color;
}

void FramebufferTransparentPal8::ExportPNM(const char* path)
{
  std::ofstream pnm;
  pnm.open(path, std::ios_base::out | std::ios_base::binary);
  pnm << "P5\n" << width << " " << height << "\n255\n";
  pnm.write((char*)buffer, static_cast<std::streamsize>(width * height * format));
  pnm.close();
}

void FramebufferTransparentPal8::ExportBMP(const char* path)
{
  auto palette = Palette::Instance();
  uint32_t bytes_pro_zeile = (width + 3) & 0xfffffffc;
  struct tagBITMAPFILEHEADER
  {
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
  } __attribute__((packed)) bmfh = {19778, bytes_pro_zeile * height + 1078, 0, 0, 1078};
  struct tagBITMAPINFOHEADER
  {
    uint32_t biSize;
    int32_t biWidth;
    int32_t biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    int32_t biXPelsPerMeter;
    int32_t biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
  } __attribute__((packed)) bmih = {40, (int32_t)width, (int32_t)height, 1, 8, 0, 0, 0, 0, 0, 0};

  std::ofstream bmp;
  bmp.open(path, std::ios_base::out | std::ios_base::binary);
  bmp.write((char*)&bmfh, sizeof(struct tagBITMAPFILEHEADER));
  bmp.write((char*)&bmih, sizeof(struct tagBITMAPINFOHEADER));

  for (int i = 0; i < 256; i++)
  {
    bmp << palette->GetColor(i).getBlue() << palette->GetColor(i).getBlue() << palette->GetColor(i).getRed() << (char)0;
  }

  for (int i = height - 1; i >= 0; i--)
  {
    bmp.write((char*)&buffer[width * i], width);
    uint32_t null = 0;
    bmp.write((char*)&null, bytes_pro_zeile - width);
  }

  bmp.close();
}

void FramebufferTransparentPal8::Clear()
{
  for (unsigned int i = 0; i < width * height; i++)
    buffer[i] = color;
}
