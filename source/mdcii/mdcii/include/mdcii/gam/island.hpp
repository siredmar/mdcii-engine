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

#ifndef _ISLAND_HPP
#define _ISLAND_HPP

#include <inttypes.h>
#include <string>

struct OreMountainData // Erzberg
{
  uint8_t ware;        // which good lays here?
  uint8_t posx;        // position on island x
  uint8_t posy;        // position on island y
  uint8_t playerflags; // which player knows this secret? (DANGER: PLAYER_MAX)
  uint8_t kind;        // which kind?
  uint8_t empty;       // empty
  uint16_t stock;      // how much of this good lays here?
};

struct Island5Data // Insel5
{
  uint8_t inselnr;
  uint8_t felderx;
  uint8_t feldery;
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
  uint32_t rohstflags;
  uint16_t filenr;
  uint16_t sizenr;
  uint8_t klimanr;
  uint8_t orginalflg;
  uint8_t duerrproz;
  uint8_t rotier;
  uint32_t seeplayerflags;
  uint32_t duerrcnt;
  uint32_t leer3;
};

class Island5
{
public:
  Island5(uint8_t* data, uint32_t length, const std::string& name);
  Island5Data island5;

private:
  std::string name;
};

struct Island3Data // Insel3
{
  uint8_t inselnr;    // ID for this island (per game)
  uint8_t breite;     // width
  uint8_t hoehe;      // height
  uint8_t a;          // TODO: unknown
  uint16_t x_pos;     // position of island x
  uint16_t y_pos;     // position of island y
  uint16_t b;         // TODO: unknown
  uint16_t c;         // TODO: unknown
  uint8_t bytes1[28]; // TODO: unknown
};

class Island3
{
public:
  Island3(uint8_t* data, uint32_t length, const std::string& name);
  Island3Data island3;

private:
  std::string name;
};


#endif // _ISLAND_HPP