
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

#include <string.h>
#include <fstream>
#include "bildspeicher_rgb24.hpp"
#include "palette.hpp"

Bildspeicher_rgb24::Bildspeicher_rgb24(uint32_t breite, uint32_t hoehe, uint32_t farbe, uint8_t* puffer, uint32_t pufferbreite)
  : Bildspeicher(breite, hoehe, 3, farbe, puffer, pufferbreite)
{
  // leer
}

void Bildspeicher_rgb24::zeichne_bsh_bild_ganz(Bsh_bild& bild, int x, int y)
{
  uint8_t ch;
  uint8_t* quelle = bild.puffer;
  uint8_t* zielzeile;
  uint8_t* ziel;

  ziel = zielzeile = this->puffer + y * this->pufferbreite + x * 3;
  int restbreite = this->pufferbreite;

  while ((ch = *(quelle++)) != 0xff)
  {
    if (ch == 0xfe)
    {
      ziel = zielzeile += restbreite;
    }
    else
    {
      ziel += ((int)ch) * 3;

      for (ch = *(quelle++); ch > 0; ch--)
      {
        int index = ((int)*(quelle++)) * 3;
        *(ziel++) = palette[index++];
        *(ziel++) = palette[index++];
        *(ziel++) = palette[index];
      }
    }
  }
}

void Bildspeicher_rgb24::zeichne_bsh_bild_partiell(Bsh_bild& bild, int x, int y)
{
  int u = 0;
  int v = 0;
  int i = 0;
  unsigned char ch;

  while ((ch = bild.puffer[i++]) != 0xff)
  {
    if (ch == 0xfe)
    {
      u = 0;
      v++;
    }
    else
    {
      u += ch;

      for (ch = bild.puffer[i++]; ch > 0; ch--, u++, i++)
      {
        if (y + v >= 0 && y + v < this->hoehe && x + u >= 0 && x + u < this->breite)
        {
          unsigned char a = bild.puffer[i];
          this->puffer[(y + v) * this->pufferbreite + (x + u) * 3] = palette[a * 3];
          this->puffer[(y + v) * this->pufferbreite + (x + u) * 3 + 1] = palette[a * 3 + 1];
          this->puffer[(y + v) * this->pufferbreite + (x + u) * 3 + 2] = palette[a * 3 + 2];
        }
      }
    }
  }
}

void Bildspeicher_rgb24::zeichne_bsh_bild(Bsh_bild& bild, int x, int y)
{
  if (x >= (int)this->breite || y >= (int)this->hoehe || x + (int)bild.breite < 0 || y + (int)bild.hoehe < 0)
    return;
  if ((x < 0) || (y < 0) || (x + (int)bild.breite > (int)this->breite) || (y + (int)bild.hoehe > (int)this->hoehe))
    zeichne_bsh_bild_partiell(bild, x, y);
  else
    zeichne_bsh_bild_ganz(bild, x, y);
}

void Bildspeicher_rgb24::zeichne_pixel(int x, int y, uint8_t farbe)
{
  if (x < 0 || y < 0 || x >= breite || y >= hoehe)
    return;
  puffer[y * pufferbreite + 3 * x] = palette[3 * farbe];
  puffer[y * pufferbreite + 3 * x + 1] = palette[3 * farbe + 1];
  puffer[y * pufferbreite + 3 * x + 2] = palette[3 * farbe + 2];
}

void Bildspeicher_rgb24::exportiere_pnm(const char* pfadname)
{
  std::ofstream pnm;
  pnm.open(pfadname, std::ios_base::out | std::ios_base::binary);
  pnm << "P6\n" << breite << " " << hoehe << "\n255\n";
  pnm.write((char*)puffer, breite * hoehe * format);
  pnm.close();
}

void Bildspeicher_rgb24::exportiere_bmp(const char* pfadname)
{
  uint32_t bytes_pro_zeile = (breite * 3 + 3) & 0xfffffffc;
  struct tagBITMAPFILEHEADER
  {
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
  } __attribute__((packed)) bmfh = {19778, bytes_pro_zeile * hoehe + 54, 0, 0, 54};
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
  } __attribute__((packed)) bmih = {40, (int32_t)breite, (int32_t)hoehe, 1, 24, 0, 0, 0, 0, 0, 0};

  std::ofstream bmp;
  bmp.open(pfadname, std::ios_base::out | std::ios_base::binary);
  bmp.write((char*)&bmfh, sizeof(struct tagBITMAPFILEHEADER));
  bmp.write((char*)&bmih, sizeof(struct tagBITMAPINFOHEADER));

  uint8_t* zeile = new uint8_t[bytes_pro_zeile];
  for (int i = hoehe - 1; i >= 0; i--)
  {
    for (int x = 0; x < breite; x++)
    {
      zeile[x * 3] = puffer[breite * 3 * i + x * 3 + 2];
      zeile[x * 3 + 1] = puffer[breite * 3 * i + x * 3 + 1];
      zeile[x * 3 + 2] = puffer[breite * 3 * i + x * 3];
    }
    bmp.write((char*)zeile, bytes_pro_zeile);
  }
  delete[] zeile;

  bmp.close();
}

void Bildspeicher_rgb24::bild_loeschen()
{
  for (int i = 0; i < breite * hoehe * 3; i += 3)
  {
    puffer[i] = farbe;
    puffer[i + 1] = farbe >> 8;
    puffer[i + 2] = farbe >> 16;
  }
}
