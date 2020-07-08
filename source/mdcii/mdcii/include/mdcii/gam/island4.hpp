// This file is part of the MDCII Game Engine.
// Copyright (C) 2020  Armin Schlegel
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

#ifndef _ISLAND4_HPP
#define _ISLAND4_HPP

#include <inttypes.h>
#include <string>

#include "chunk.hpp"
#include "island.hpp"

struct Island4Data
{
  uint8_t fileNumber;
  uint8_t width;
  uint8_t height;
  uint8_t strtduerrflg : 1;
  uint8_t nofixflg : 1;
  uint8_t vulkanflg : 1;
  uint16_t posx;
  uint16_t posy;
  uint16_t hirschreviercnt;
  uint16_t speedcnt;
  uint8_t stadtplayernr[11];
  uint8_t vulkancnt;
  uint8_t schatzflg;
  uint8_t rohstanz;
  uint8_t eisencnt;
  uint8_t playerflags;
  OreMountainData eisenberg[4];
  OreMountainData vulkanberg[4];
  Fertility fertility;
  uint16_t empty; // always 0xFFFF
  IslandSize size;
  IslandClimate climate;
  IslandModified modifiedFlag; // flag that indicates if the island is `original` (empty INSELHAUS chunk) or `modified` (filled INSELHAUS chunk).
  uint8_t duerrproz;
  uint8_t rotier;
  uint32_t seeplayerflags;
  uint32_t duerrcnt;
  uint32_t leer3;
};

class Island4
{
public:
  Island4()
  {
  }
  explicit Island4(uint8_t* data, uint32_t length, const std::string& name);
  Island4Data island;

private:
  std::string name;
};

#endif // _ISLAND4_HPP