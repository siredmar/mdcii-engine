
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

#include <iostream>
#include "grafiken.hpp"
#include "grafiken_nina.hpp"
#include "grafiken_vanilla.hpp"
#include "version.hpp"

Grafiken::Grafiken(Anno_version version)
{

  std::vector<Grafikinfo_mit_index>* grafikinfo_mit_index;

  switch (version)
  {
    case Anno_version::NINA:
      std::cout << "[INFO] Benutze Grafiken fuer Version NINA" << std::endl;
      grafikinfo_mit_index = &grafiken_nina;
      break;
    case Anno_version::VANILLA:
    default:
      std::cout << "[INFO] Benutze Grafiken fuer Version VANILLA" << std::endl;
      grafikinfo_mit_index = &grafiken_vanilla;
      break;
  }

  for (auto e : *grafikinfo_mit_index)
  {
    index[e.bebauung] = e.grafikindex;
  }
}

int Grafiken::grafik_zu(uint16_t i)
{
  auto grafik = index.find(i);
  if (grafik != index.end())
    return grafik->second;
  else
    return -1;
}
