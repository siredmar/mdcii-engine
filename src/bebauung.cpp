
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

#include <vector>
#include <iostream>

#include "bebauung.hpp"
#include "bebauung_vanilla.hpp"

Bebauung::Bebauung(Anno_version version)
{
  std::vector<Bebauungsinfo_mit_index>* bebauungsinfo_mit_index;

  switch (version)
  {
    // TODO: Bebauungen fuer verschiedene Versionen definieren und hier benutzen
    case Anno_version::NINA:
      std::cout << "[INFO] Benutze Bebauung fuer Version NINA" << std::endl;
      bebauungsinfo_mit_index = &bebauung_vanilla;
      break;
    case Anno_version::VANILLA:
    default:
      std::cout << "[INFO] Benutze Bebauung fuer Version VANILLA" << std::endl;
      bebauungsinfo_mit_index = &bebauung_vanilla;
      break;
  }

  for (auto e : *bebauungsinfo_mit_index)
  {
    index[e.index] = e.bebauung;
  }
}

Bebauungsinfo* Bebauung::info_zu(uint16_t i)
{
  auto info = index.find(i);
  if (info != index.end())
  {
    return &info->second;
  }
  else
  {
    return nullptr;
  }
}