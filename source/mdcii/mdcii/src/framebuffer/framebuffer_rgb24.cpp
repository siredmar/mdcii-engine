
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

#include "framebuffer/framebuffer_rgb24.hpp"
#include "framebuffer/palette.hpp"
#include <fstream>
#include <string.h>

FramebufferRgb24::FramebufferRgb24(uint32_t width, uint32_t height, uint32_t color, uint8_t* buffer, uint32_t bufferLength)
  : Framebuffer(width, height, 3, color, buffer, bufferLength)
{
  Clear();
}

void FramebufferRgb24::DrawBshImageFull(BshImage& image, int x, int y)
{
  auto palette = Palette::Instance();
  uint8_t ch;
  uint8_t* quelle = image.buffer;
  uint8_t* zielzeile;
  uint8_t* target;

  target = zielzeile = this->buffer + y * this->bufferLength + x * 3;
  int restbreite = this->bufferLength;

  while ((ch = *(quelle++)) != 0xff)
  {
    if (ch == 0xfe)
    {
      target = zielzeile += restbreite;
    }
    else
    {
      target += ((int)ch) * 3;
      for (ch = *(quelle++); ch > 0; ch--)
      {
        int index = ((int)*(quelle++)) * 3;
        *(target++) = palette->Index(index++);
        *(target++) = palette->Index(index++);
        *(target++) = palette->Index(index);
      }
    }
  }
}

void FramebufferRgb24::DrawBshImagePartial(BshImage& image, int x, int y)
{
  auto palette = Palette::Instance();

  int u = 0;
  int v = 0;
  int i = 0;
  unsigned char ch;

  while ((ch = image.buffer[i++]) != 0xff)
  {
    if (ch == 0xfe)
    {
      u = 0;
      v++;
    }
    else
    {
      u += ch;

      for (ch = image.buffer[i++]; ch > 0; ch--, u++, i++)
      {
        if (y + v >= 0 && y + v < static_cast<int>(height) && x + u >= 0 && x + u < static_cast<int>(width))
        {
          unsigned char a = image.buffer[i];
          this->buffer[(y + v) * this->bufferLength + (x + u) * 3] = palette->GetColor(a).getRed();
          this->buffer[(y + v) * this->bufferLength + (x + u) * 3 + 1] = palette->GetColor(a).getGreen();
          this->buffer[(y + v) * this->bufferLength + (x + u) * 3 + 2] = palette->GetColor(a).getBlue();
        }
      }
    }
  }
}

void FramebufferRgb24::DrawBshImage(BshImage& image, int x, int y)
{
  if (x >= static_cast<int>(width) || y >= static_cast<int>(height) || x + static_cast<int>(image.width) < 0 || y + static_cast<int>(image.height) < 0)
  {
    return;
  }
  if ((x < 0) || (y < 0) || (x + static_cast<int>(image.width) > static_cast<int>(width)) || (y + static_cast<int>(image.height) > static_cast<int>(height)))
  {
    DrawBshImagePartial(image, x, y);
  }
  else
  {
    DrawBshImageFull(image, x, y);
  }
}

void FramebufferRgb24::DrawPixel(int x, int y, uint8_t color)
{
  auto palette = Palette::Instance();
  if ((x < 0) || (y < 0) || (x >= static_cast<int>(width)) || (y >= static_cast<int>(height)))
  {
    return;
  }
  buffer[static_cast<unsigned int>(y * bufferLength + 3 * x)] = palette->GetColor(color).getRed();
  buffer[static_cast<unsigned int>(y * bufferLength + 3 * x + 1)] = palette->GetColor(color).getGreen();
  buffer[static_cast<unsigned int>(y * bufferLength + 3 * x + 2)] = palette->GetColor(color).getBlue();
}

void FramebufferRgb24::ExportPNM(const char* path)
{
  std::ofstream pnm;
  pnm.open(path, std::ios_base::out | std::ios_base::binary);
  pnm << "P6\n" << width << " " << height << "\n255\n";
  pnm.write((char*)buffer, static_cast<std::streamsize>(width * height * format));
  pnm.close();
}

void FramebufferRgb24::ExportBMP(const char* path)
{
  uint32_t bytes_pro_zeile = (width * 3 + 3) & 0xfffffffc;
  struct tagBITMAPFILEHEADER
  {
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
  } __attribute__((packed)) bmfh = {19778, bytes_pro_zeile * height + 54, 0, 0, 54};
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
  } __attribute__((packed)) bmih = {40, (int32_t)width, (int32_t)height, 1, 24, 0, 0, 0, 0, 0, 0};

  std::ofstream bmp;
  bmp.open(path, std::ios_base::out | std::ios_base::binary);
  bmp.write((char*)&bmfh, sizeof(struct tagBITMAPFILEHEADER));
  bmp.write((char*)&bmih, sizeof(struct tagBITMAPINFOHEADER));

  uint8_t* zeile = new uint8_t[bytes_pro_zeile];
  for (int i = static_cast<int>(height) - 1; i >= 0; i--)
  {
    for (int x = 0; x < static_cast<int>(width); x++)
    {
      zeile[x * 3] = buffer[width * 3 * i + x * 3 + 2];
      zeile[x * 3 + 1] = buffer[width * 3 * i + x * 3 + 1];
      zeile[x * 3 + 2] = buffer[width * 3 * i + x * 3];
    }
    bmp.write((char*)zeile, bytes_pro_zeile);
  }
  delete[] zeile;

  bmp.close();
}

void FramebufferRgb24::Clear()
{
  for (unsigned int i = 0; i < width * height * 3; i += 3)
  {
    buffer[i] = color;
    buffer[i + 1] = color >> 8;
    buffer[i + 2] = color >> 16;
  }
}
