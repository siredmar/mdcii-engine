
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

#include "bildspeicher_trans_pal8.hpp"
#include "palette.hpp"
#include <fstream>
#include <ios>
#include <iostream>
#include <string.h>

Bildspeicher_trans_pal8::Bildspeicher_trans_pal8(uint32_t breite, uint32_t hoehe, uint32_t farbe, uint8_t* puffer, uint32_t pufferbreite, uint8_t transparent)
  : Bildspeicher(breite, hoehe, 1, farbe, puffer, pufferbreite)
  , transparent(transparent)
{
  bild_loeschen();
}

void Bildspeicher_trans_pal8::zeichne_bsh_bild_ganz(Bsh_bild& bild, int x, int y)
{
  uint8_t* quelle = bild.puffer;
  uint8_t* ziel;

  ziel = this->puffer;
  // Read until we reach the end marker
  while (true)
  {
    int numAlpha = *(quelle++);

    // End marker
    if (numAlpha == 255)
    {
      int rest = this->pufferbreite - x;
      for (int i = x; i < x + rest; i++)
      {
        puffer[y * pufferbreite + i] = transparent;
      }
      break;
    }

    // End of row
    if (numAlpha == 254)
    {
      int rest = this->pufferbreite - x;
      for (int i = x; i < x + rest; i++)
      {
        puffer[y * pufferbreite + i] = transparent;
      }
      x = 0;
      y++;
      continue;
    }

    // Pixel data
    for (int i = 0; i < numAlpha; i++)
    {
      puffer[y * pufferbreite + x] = transparent;
      x++;
    }
    int numPixels = *(quelle++);
    for (int i = 0; i < numPixels; i++)
    {
      int colourIndex = *(quelle++);
      puffer[y * pufferbreite + x] = colourIndex;
      x++;
    }
  }
}

void Bildspeicher_trans_pal8::zeichne_bsh_bild(Bsh_bild& bild, int x, int y)
{
  if (x >= (int)this->breite || y >= (int)this->hoehe || x + (int)bild.breite < 0 || y + (int)bild.hoehe < 0)
    return;
  zeichne_bsh_bild_ganz(bild, x, y);
}

void Bildspeicher_trans_pal8::zeichne_pixel(int x, int y, uint8_t farbe)
{
  if (x < 0 || y < 0 || x >= breite || y >= hoehe)
    return;
  puffer[y * pufferbreite + x] = farbe;
}

void Bildspeicher_trans_pal8::exportiere_pnm(const char* pfadname)
{
  std::ofstream pnm;
  pnm.open(pfadname, std::ios_base::out | std::ios_base::binary);
  pnm << "P5\n" << breite << " " << hoehe << "\n255\n";
  pnm.write((char*)puffer, static_cast<std::streamsize>(breite * hoehe * format));
  pnm.close();
}

void Bildspeicher_trans_pal8::exportiere_bmp(const char* pfadname)
{
  auto palette = Palette::instance();
  uint32_t bytes_pro_zeile = (breite + 3) & 0xfffffffc;
  struct tagBITMAPFILEHEADER
  {
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
  } __attribute__((packed)) bmfh = {19778, bytes_pro_zeile * hoehe + 1078, 0, 0, 1078};
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
  } __attribute__((packed)) bmih = {40, (int32_t)breite, (int32_t)hoehe, 1, 8, 0, 0, 0, 0, 0, 0};

  std::ofstream bmp;
  bmp.open(pfadname, std::ios_base::out | std::ios_base::binary);
  bmp.write((char*)&bmfh, sizeof(struct tagBITMAPFILEHEADER));
  bmp.write((char*)&bmih, sizeof(struct tagBITMAPINFOHEADER));

  for (int i = 0; i < 256; i++)
  {
    bmp << palette->getColor(i).getBlue() << palette->getColor(i).getBlue() << palette->getColor(i).getRed() << (char)0;
  }

  for (int i = hoehe - 1; i >= 0; i--)
  {
    bmp.write((char*)&puffer[breite * i], breite);
    uint32_t null = 0;
    bmp.write((char*)&null, bytes_pro_zeile - breite);
  }

  bmp.close();
}

void Bildspeicher_trans_pal8::bild_loeschen()
{
  for (int i = 0; i < breite * hoehe; i++)
    puffer[i] = farbe;
}


uint8_t* Bildspeicher_trans_pal8::get_buffer()
{
  return puffer;
}