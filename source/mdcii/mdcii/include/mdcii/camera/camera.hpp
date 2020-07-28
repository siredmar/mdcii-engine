
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
#include <memory>

#include "bsh/bshreader.hpp"
#include "framebuffer/framebuffer.hpp"
#include "welt.hpp"

/* GFX/ MGFX/ Sgfx/  EFFEKTE.BSH MAEHER.BSH NUMBERS.BSH SHIP.BSH SOLDAT.BSH STADTFLD.BSH TIERE.BSH TRAEGER.BSH */

class Kamera
{
public:
  explicit Kamera();

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

  void auf_bildschirm(Framebuffer& fb, int karte_x, int karte_y, int& bildschirm_x, int& bildschirm_y);
  void auf_bildschirm(Framebuffer& fb, int karte_x, int karte_y, int karte_z, int& bildschirm_x, int& bildschirm_y, int& bildschirm_z);
  void auf_bildschirm_256(Framebuffer& fb, int karte_x, int karte_y, int karte_z, int& bildschirm_x, int& bildschirm_y, int& bildschirm_z);
  void auf_karte(Framebuffer& fb, int bildschirm_x, int bildschirm_y, int& karte_x, int& karte_y);

  void zeichne_bild(Framebuffer& fb, Welt& welt, int maus_x, int maus_y);

private:
  std::shared_ptr<Buildings> buildings;
  uint16_t xpos, ypos;
  uint8_t drehung, vergroesserung;

  static const int x_raster[3];
  static const int y_raster[3];
  static const int grundhoehe[3];

  std::shared_ptr<BshReader> effekte_bsh[3];
  std::shared_ptr<BshReader> maeher_bsh[3];
  std::shared_ptr<BshReader> numbers_bsh[3];
  std::shared_ptr<BshReader> ship_bsh[3];
  std::shared_ptr<BshReader> soldat_bsh[3];
  std::shared_ptr<BshReader> stadtfld_bsh[3];
  std::shared_ptr<BshReader> tiere_bsh[3];
  std::shared_ptr<BshReader> traeger_bsh[3];
  std::shared_ptr<ZeiReader> zei;
};

#endif
