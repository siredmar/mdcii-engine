
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

#ifndef BILDSPEICHER_PAL8_HPP
#define BILDSPEICHER_PAL8_HPP

#include "bildspeicher.hpp"
#include "bsh_leser.hpp"
#include <inttypes.h>

class Bildspeicher_pal8 : public Bildspeicher
{
  uint8_t dunkel[256];

  void zeichne_bsh_bild_ganz(Bsh_bild& bild, int x, int y);
  void zeichne_bsh_bild_partiell(Bsh_bild& bild, int x, int y);

public:
  Bildspeicher_pal8(uint32_t breite, uint32_t hoehe, uint32_t farbe = 0, uint8_t* puffer = NULL, uint32_t pufferbreite = 0);
  void zeichne_bsh_bild(Bsh_bild& bild, int x, int y);
  void zeichne_pixel(int x, int y, uint8_t farbe);
  void exportiere_pnm(const char* pfadname);
  void exportiere_bmp(const char* pfadname);
  void bild_loeschen();
  uint8_t* get_buffer();
};

#endif