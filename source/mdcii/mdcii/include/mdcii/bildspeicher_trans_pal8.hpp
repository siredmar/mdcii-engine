
// This file is part of the MDCII Game Engine.
// Copyright (C) 2020 Armin Schlegel
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

#ifndef BILDSPEICHER_TRANS_PAL8_HPP
#define BILDSPEICHER_TRANS_PAL8_HPP

#include <inttypes.h>

#include "bildspeicher.hpp"
#include "bsh_leser.hpp"


class Bildspeicher_trans_pal8 : public Bildspeicher
{
public:
  Bildspeicher_trans_pal8(uint32_t breite, uint32_t hoehe, uint32_t farbe = 0, uint8_t* puffer = NULL, uint32_t pufferbreite = 0, uint8_t transparent = 253);
  void zeichne_bsh_bild(Bsh_bild& bild, int x, int y);
  void zeichne_pixel(int x, int y, uint8_t farbe);
  void exportiere_pnm(const char* pfadname);
  void exportiere_bmp(const char* pfadname);
  void bild_loeschen();
  uint8_t* get_buffer();

private:
  uint8_t transparent;
  uint8_t dunkel[256];

  void zeichne_bsh_bild_ganz(Bsh_bild& bild, int x, int y);
};

#endif