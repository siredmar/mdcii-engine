
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

#ifndef KAMERA_HPP
#define KAMERA_HPP

#include <inttypes.h>
#include "bsh_leser.hpp"
#include "bildspeicher.hpp"
#include "welt.hpp"

/* GFX/ MGFX/ Sgfx/  EFFEKTE.BSH MAEHER.BSH NUMBERS.BSH SHIP.BSH SOLDAT.BSH STADTFLD.BSH TIERE.BSH TRAEGER.BSH */

class Kamera
{
public:
  Kamera(std::shared_ptr<Haeuser> haeuser);
  void gehe_zu(uint16_t x, uint16_t y);
  void nach_rechts();
  void nach_links();
  void nach_oben();
  void nach_unten();
  void vergroessern();
  void verkleinern();
  void setze_vergroesserung(uint8_t vergroesserung);
  void rechts_drehen();
  void links_drehen();

  void auf_bildschirm(Bildspeicher& bs, int karte_x, int karte_y, int& bildschirm_x, int& bildschirm_y);
  void auf_bildschirm(Bildspeicher& bs, int karte_x, int karte_y, int karte_z, int& bildschirm_x, int& bildschirm_y, int& bildschirm_z);
  void auf_bildschirm_256(Bildspeicher& bs, int karte_x, int karte_y, int karte_z, int& bildschirm_x, int& bildschirm_y, int& bildschirm_z);
  void auf_karte(Bildspeicher& bs, int bildschirm_x, int bildschirm_y, int& karte_x, int& karte_y);

  void zeichne_bild(Bildspeicher& bs, Welt& welt, int maus_x, int maus_y);

private:
  std::shared_ptr<Haeuser> haeuser;
  uint16_t xpos, ypos;
  uint8_t drehung, vergroesserung;

  static const int x_raster[3];
  static const int y_raster[3];
  static const int grundhoehe[3];

  Bsh_leser* effekte_bsh[3];
  Bsh_leser* maeher_bsh[3];
  Bsh_leser* numbers_bsh[3];
  Bsh_leser* ship_bsh[3];
  Bsh_leser* soldat_bsh[3];
  Bsh_leser* stadtfld_bsh[3];
  Bsh_leser* tiere_bsh[3];
  Bsh_leser* traeger_bsh[3];
  Zei_leser* zei;
};

#endif
