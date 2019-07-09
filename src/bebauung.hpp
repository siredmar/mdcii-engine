
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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
// USA.

#ifndef BEBAUUNG_HPP
#define BEBAUUNG_HPP

#include <map>
#include <string>

#include "version.hpp"

struct Bebauungsinfo
{
  uint8_t breite;
  uint8_t hoehe;
  uint8_t richtungen;
  uint8_t ani_schritte;
  uint8_t grundhoehe;
  uint8_t bauhoehe;
  uint8_t lagerstaende;
  uint8_t kategorie;
};

struct Bebauungsinfo_mit_index
{
  uint16_t index;
  Bebauungsinfo bebauung;
};

class Bebauung
{
public:
  Bebauung(Anno_version version);
  Bebauungsinfo* info_zu(uint16_t i);
  std::map<uint16_t, Bebauungsinfo> init(Anno_version version);

private:
  std::map<uint16_t, Bebauungsinfo> index;
};

#endif
