
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

#ifndef BILDSPEICHER_HPP
#define BILDSPEICHER_HPP

#include <inttypes.h>

#include "bsh_leser.hpp"
#include "zei_leser.hpp"

class Bildspeicher
{
protected:
  uint8_t* puffer;
  uint8_t puffer_freigeben;
  uint32_t pufferbreite;
  uint32_t format;
  uint32_t farbe;

  uint8_t indextabelle_schriftfarbe[256];

public:
  uint32_t breite;
  uint32_t hoehe;

  Bildspeicher(uint32_t breite, uint32_t hoehe, uint32_t format = 1, uint32_t farbe = 0, uint8_t* puffer = NULL, uint32_t pufferbreite = 0);
  ~Bildspeicher();
  virtual void zeichne_bsh_bild(Bsh_bild& bild, int x, int y);
  void zeichne_bsh_bild_oz(Bsh_bild& bild, int x, int y);
  virtual void zeichne_bsh_bild_sp(Bsh_bild& bild, int x, int y, int sx, int sy, bool& schnitt);
  void zeichne_bsh_bild_sp_oz(Bsh_bild& bild, int x, int y, int sx, int sy, bool& schnitt);
  virtual void zeichne_pixel(int x, int y, uint8_t farbe) = 0;
  virtual void zeichne_rechteck(int x1, int y1, int x2, int y2, uint8_t farbe);
  virtual void zeichne_linie(int x1, int y1, int x2, int y2, uint8_t farbe, uint8_t muster = 0xff);
  virtual void zeichne_zei_zeichen(Zei_zeichen& zeichen, int x, int y);
  void zeichne_string(Zei_leser& zei_leser, std::string s, int x, int y);
  void setze_schriftfarbe(uint8_t schrift, uint8_t schatten);
  virtual void exportiere_pnm(const char* pfadname);
  virtual void exportiere_bmp(const char* pfadname);
  virtual void bild_loeschen();
  virtual uint8_t* get_buffer();
};

#endif