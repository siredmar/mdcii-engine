
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

#include "framebuffer/framebuffer.hpp"

Framebuffer::Framebuffer(uint32_t width, uint32_t height, uint32_t format, uint32_t color, uint8_t* buffer, uint32_t bufferLength)
{
  Init(width, height, format, color, buffer, bufferLength);

  // initialisiere die Indextabelle, in der an Positionen 1 und 7 die Schrift- bzw. Schattenfarbe erwartet wird
  for (int i = 0; i < 256; i++)
  {
    fontColorTable[i] = i;
  }
}

Framebuffer::~Framebuffer()
{
  Uninit();
}

void Framebuffer::Init(uint32_t width, uint32_t height, uint32_t format, uint32_t color, uint8_t* buffer, uint32_t bufferLength)
{
  this->width = width;
  this->height = height;
  this->format = format;
  this->color = color;
  this->bufferLength = (bufferLength >= width * format) ? bufferLength : (width * format);
  if (buffer == NULL)
  {
    this->buffer = new uint8_t[this->bufferLength * height];
    freeBuffer = 1;
  }
  else
  {
    this->buffer = buffer;
    freeBuffer = 0;
  }
}

void Framebuffer::Uninit()
{
  if (freeBuffer)
    //  deepcode ignore CppDoubleFree: Used by deconstructor and Resize method
    delete[] buffer;
}

void Framebuffer::Resize(uint32_t width, uint32_t height, uint32_t format, uint32_t color, uint8_t* buffer, uint32_t bufferLength)
{
  Uninit();
  Init(width, height, format, color, buffer, bufferLength);
}

void Framebuffer::DrawBshImage([[maybe_unused]] BshImage& image, [[maybe_unused]] int x, [[maybe_unused]] int y)
{
  // in einer Unterklasse implementiert
}

void Framebuffer::zeichne_bsh_bild_oz(BshImage& image, int x, int y)
{
  DrawBshImage(image, x - image.width / 2, y - image.height);
}

void Framebuffer::zeichne_bsh_bild_sp(BshImage& image, int x, int y, int sx, int sy, bool& schnitt)
{
  schnitt = false;
  sx -= x;
  sy -= y;

  if (sx < 0 || sy < 0 || sx >= static_cast<int>(image.width) || sy >= static_cast<int>(image.height))
  {
    DrawBshImage(image, x, y);
  }
  else
  {
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
          DrawPixel(x + u, y + v, image.buffer[i]);
          if (u == sx && v == sy)
            schnitt = true;
        }
      }
    }
  }
}

void Framebuffer::zeichne_bsh_bild_sp_oz(BshImage& image, int x, int y, int sx, int sy, bool& schnitt)
{
  zeichne_bsh_bild_sp(image, x - image.width / 2, y - image.height, sx, sy, schnitt);
}

void Framebuffer::DrawRectangle(int x1, int y1, int x2, int y2, uint8_t color)
{
  if (x1 < 0)
    x1 = 0;
  if (y1 < 0)
    y1 = 0;
  if (x2 >= static_cast<int>(width))
    x2 = static_cast<int>(width) - 1;
  if (y2 >= static_cast<int>(height))
    y2 = static_cast<int>(height) - 1;

  for (int y = y1; y <= y2; y++)
  {
    for (int x = x1; x <= x2; x++)
    {
      DrawPixel(x, y, color);
    }
  }
}

void Framebuffer::DrawLine(int x1, int y1, int x2, int y2, uint8_t color, uint8_t pattern)
{
  int xdiff = abs(x2 - x1);
  int ydiff = abs(y2 - y1);

  int ix = (x2 > x1) - (x2 < x1);
  int iy = (y2 > y1) - (y2 < y1);

  int cx = x1;
  int cy = y1;

  if (xdiff >= ydiff)
  {
    int s = xdiff >> 1;
    do
    {
      if ((pattern = pattern >> 1 | pattern << 7) & 0x80)
      {
        DrawPixel(cx, cy, color);
      }
      cx += ix;
      s -= ydiff;
      if (s <= 0)
      {
        s += xdiff;
        cy += iy;
      }
    } while (cx != x2);
  }
  else
  {
    int s = ydiff >> 1;
    do
    {
      if ((pattern = pattern >> 1 | pattern << 7) & 0x80)
      {
        DrawPixel(cx, cy, color);
      }
      cy += iy;
      s -= xdiff;
      if (s <= 0)
      {
        s += ydiff;
        cx += ix;
      }
    } while (cy != y2);
  }

  if ((pattern = pattern >> 1 | pattern << 7) & 0x80)
  {
    DrawPixel(x2, y2, color);
  }
}

void Framebuffer::DrawZeiCharacter(ZeiCharacter& character, int x, int y)
{
  int u = 0;
  int v = 0;
  int i = 0;

  while (1)
  {
    unsigned char ch = character.buffer[i++];
    if (ch == 0xff)
    {
      break;
    }
    if (ch == 0xfe)
    {
      u = 0;
      v++;
    }
    else
    {
      u += ch;

      for (ch = character.buffer[i++]; ch > 0; ch--, u++, i++)
      {
        DrawPixel(x + u, y + v, fontColorTable[character.buffer[i]]);
      }
    }
  }
}


void Framebuffer::DrawString(ZeiReader& zei, std::string s, int x, int y)
{
  for (char ch : s)
  {
    if (((ch - ' ') < 0) || (static_cast<uint32_t>(ch - ' ') >= zei.Count()))
      continue;
    ZeiCharacter& zz = zei.GetBshImage(ch - ' ');
    DrawZeiCharacter(zz, x, y);
    x += zz.width;
  }
}

void Framebuffer::DrawString(ZeiReader& zei, std::wstring s, int x, int y)
{
  for (auto ch : s)
  {
    if (((ch - ' ') < 0) || (static_cast<uint32_t>(ch - ' ') >= zei.Count()))
      continue;
    ZeiCharacter& zz = zei.GetBshImage(ch - ' ');
    DrawZeiCharacter(zz, x, y);
    x += zz.width;
  }
}

void Framebuffer::SetFontColor(uint8_t font, uint8_t shadow)
{
  fontColorTable[1] = font;
  fontColorTable[7] = shadow;
}

void Framebuffer::FillWithColor(uint8_t color)
{
  memset((uint8_t*)buffer, color, static_cast<size_t>(height * width));
}

void Framebuffer::ExportPNM([[maybe_unused]] const char* path)
{
  // in einer Unterklasse implementiert
}

void Framebuffer::ExportBMP([[maybe_unused]] const char* path)
{
  // in einer Unterklasse implementiert
}

void Framebuffer::Clear()
{
  // in einer Unterklasse implementiert
}
