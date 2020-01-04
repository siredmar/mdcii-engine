
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
#include "bildspeicher_pal8.hpp"
#include "palette.hpp"

Bildspeicher_pal8::Bildspeicher_pal8(uint32_t breite, uint32_t hoehe, uint32_t farbe, uint8_t* puffer, uint32_t pufferbreite)
  : Bildspeicher(breite, hoehe, 1, farbe, puffer, pufferbreite)
{
  // erzeuge Index der Paletteneinträge mit der jeweils halben Helligkeit
  for (int i = 0; i < 256; i++)
  {
    uint8_t r = palette[3 * i] >> 1;
    uint8_t g = palette[3 * i + 1] >> 1;
    uint8_t b = palette[3 * i + 2] >> 1;

    int mindiff = 0x7fffffff;
    for (int j = 0; j < 256; j++)
    {
      int diff = abs((int)palette[3 * j] - r) + abs((int)palette[3 * j + 1] - g) + abs((int)palette[3 * j + 2] - b);
      if (diff < mindiff)
      {
        dunkel[i] = j;
        mindiff = diff;
      }
    }
  }
}

void Bildspeicher_pal8::zeichne_bsh_bild_ganz(Bsh_bild& bild, int x, int y)
{
  uint8_t ch;
  uint8_t* quelle = bild.puffer;
  uint8_t* zielzeile;
  uint8_t* ziel;

  ziel = zielzeile = this->puffer + y * this->pufferbreite + x;
  int restbreite = this->pufferbreite;

  while ((ch = *(quelle++)) != 0xff)
  {
    if (ch == 0xfe)
    {
      ziel = zielzeile += restbreite;
    }
    else
    {
      ziel += ch;

      for (ch = *(quelle++); ch > 0; ch--)
        *(ziel++) = *(quelle++);
    }
  }
}

void Bildspeicher_pal8::zeichne_bsh_bild_partiell(Bsh_bild& bild, int x, int y)
{
  int u = 0;
  int v = 0;
  int i = 0;
  unsigned char ch;

  uint8_t* quelle = bild.puffer;
  uint8_t* zielzeile;
  uint8_t* ziel = zielzeile = this->puffer + y * (int)this->pufferbreite + x;
  int restbreite = this->pufferbreite;

  if (x >= 0 && x + bild.breite < this->breite)
  {
    uint8_t* anfang = this->puffer;
    uint8_t* ende = this->puffer + this->pufferbreite * this->hoehe;

    while (ziel < anfang)
    {
      ch = *(quelle++);
      if (ch == 0xff)
        return;
      if (ch == 0xfe)
      {
        ziel = zielzeile += restbreite;
        if (ziel >= ende)
          return;
      }
      else
      {
        ziel += ch;

        ch = *(quelle++);
        quelle += ch;
        ziel += ch;
      }
    }
    while ((ch = *(quelle++)) != 0xff)
    {
      if (ch == 0xfe)
      {
        ziel = zielzeile += restbreite;
        if (ziel >= ende)
          return;
      }
      else
      {
        ziel += ch;

        for (ch = *(quelle++); ch > 0; ch--)
          *(ziel++) = *(quelle++);
      }
    }
  }
  else
  {
    while ((ch = *(quelle++)) != 0xff)
    {
      if (ch == 0xfe)
      {
        ziel = zielzeile += restbreite;
        u = 0;
        v++;
        if (y + v >= (int)this->hoehe)
          return;
      }
      else
      {
        u += ch;
        ziel += ch;

        ch = *(quelle++);
        if (y + v >= 0)
        {
          for (; ch > 0; ch--, u++, quelle++, ziel++)
            if (x + u >= 0 && x + u < this->breite)
              *ziel = *quelle;
        }
        else
        {
          u += ch;
          quelle += ch;
          ziel += ch;
          ch = 0;
        }
      }
    }
  }
}

void Bildspeicher_pal8::zeichne_bsh_bild(Bsh_bild& bild, int x, int y)
{
  if (x >= (int)this->breite || y >= (int)this->hoehe || x + (int)bild.breite < 0 || y + (int)bild.hoehe < 0)
    return;
  if ((x < 0) || (y < 0) || (x + (int)bild.breite > (int)this->breite) || (y + (int)bild.hoehe > (int)this->hoehe))
    zeichne_bsh_bild_partiell(bild, x, y);
  else
    zeichne_bsh_bild_ganz(bild, x, y);
}

void Bildspeicher_pal8::zeichne_pixel(int x, int y, uint8_t farbe)
{
  if (x < 0 || y < 0 || x >= breite || y >= hoehe)
    return;
  puffer[y * pufferbreite + x] = farbe;
}

void Bildspeicher_pal8::exportiere_pnm(const char* pfadname)
{
  std::ofstream pnm;
  pnm.open(pfadname, std::ios_base::out | std::ios_base::binary);
  pnm << "P5\n" << breite << " " << hoehe << "\n255\n";
  pnm.write((char*)puffer, breite * hoehe * format);
  pnm.close();
}

void Bildspeicher_pal8::exportiere_bmp(const char* pfadname)
{
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
    bmp << palette[i * 3 + 2] << palette[i * 3 + 1] << palette[i * 3] << (char)0;
  }

  for (int i = hoehe - 1; i >= 0; i--)
  {
    bmp.write((char*)&puffer[breite * i], breite);
    uint32_t null = 0;
    bmp.write((char*)&null, bytes_pro_zeile - breite);
  }

  bmp.close();
}

void Bildspeicher_pal8::bild_loeschen()
{
  for (int i = 0; i < breite * hoehe; i++)
    puffer[i] = farbe;
}
