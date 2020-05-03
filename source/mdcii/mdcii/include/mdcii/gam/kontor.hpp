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

#ifndef _KONTOR_HPP_
#define _KONTOR_HPP_

#include <inttypes.h>
#include <string>
#include <vector>

struct KontorWare
{
  uint32_t vkpreis : 10;
  uint32_t ekpreis : 10;
  uint32_t vkflg : 1;
  uint32_t ekflg : 1;
  uint32_t lagerres : 16;
  uint32_t ownlager : 16;
  uint32_t minlager : 16;
  uint32_t bedarf : 16;
  uint32_t lager;
  uint32_t hausid : 16;
};

#define MAXKONTWARESAVE 50
struct Kontor2Data
{
  uint32_t inselnr : 8;
  uint32_t posx : 8;
  uint32_t posy : 8;
  uint32_t stadtnr : 4;
  KontorWare waren[50]; //  ACHTUNG falls WARE_MAX > 50
};

class Kontor2
{
public:
  Kontor2(uint8_t* data, uint32_t length, const std::string& name);
  std::vector<Kontor2Data> kontors;

private:
  std::string name;
};


#endif // _KONTOR_HPP_