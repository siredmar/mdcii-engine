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

#include <cstring>
#include <memory>

#include "gam/chunk.hpp"
#include "gam/island.hpp"

Island5::Island5(uint8_t* data, uint32_t length, const std::string& name)
  : name(name)
{
  memcpy((char*)&island5, data, length);
}

void Island5::setIslandHouse(std::shared_ptr<Chunk> c)
{
  islandHouse = IslandHouse(c->chunk.data, c->chunk.length, "INELHAUS");
}

void Island5::setWarehouse2(std::shared_ptr<Chunk> c)
{
  warehouse2 = Warehouse2(c->chunk.data, c->chunk.length, "KONTOR2");
}

void Island5::setProductionList(std::shared_ptr<Chunk> c)
{
  productionList = ProductionList(c->chunk.data, c->chunk.length, "PRODLIST2");
}

void Island5::setDeer(std::shared_ptr<Chunk> c)
{
  deer = Deer(c->chunk.data, c->chunk.length, "HIRSCH2");
}

Island3::Island3(uint8_t* data, uint32_t length, const std::string& name)
  : name(name)
{
  memcpy((char*)&island3, data, length);
}