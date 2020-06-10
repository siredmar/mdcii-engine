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
#include <ctime>
#include <fstream>
#include <memory>
#include <sstream>

#include <boost/format.hpp>
#include <boost/random.hpp>

#include "gam/chunk.hpp"
#include "gam/island.hpp"
#include "gam/islandhouse.hpp"


Island5::Island5(uint8_t* data, uint32_t length, const std::string& name)
  : name(name)
  , files(Files::instance())
{
  memcpy((char*)&island, data, length);
}

void Island5::setIslandNumber(uint8_t number)
{
  island.islandNumber = number;
}

void Island5::setIslandFile(uint16_t fileNumber)
{
  island.fileNumber = fileNumber;
}

void Island5::addIslandHouse(std::shared_ptr<Chunk> c)
{
  islandHouse.push_back(IslandHouse(c->chunk.data, c->chunk.length, c->chunk.name.c_str()));
}

void Island5::addIslandHouse(IslandHouse i)
{
  islandHouse.push_back(i);
}

std::string Island5::islandFileName(IslandSize size, uint8_t islandNumber, IslandClimate climate)
{
  return boost::str(boost::format("%s/%s%02d.scp") % islandClimateMap[climate] % islandSizeMap[size] % (int)islandNumber);
}

IslandClimate Island5::randomIslandClimate()
{
  std::time_t now = std::time(0);
  boost::random::mt19937 gen{static_cast<std::uint32_t>(now)};
  return static_cast<IslandClimate>(gen() % static_cast<uint32_t>(IslandClimate::Any));
}

void Island5::finalize()
{
  // island is unmodified, load bottom islandHouse from island file
  if (island.modifiedFlag == IslandModified::False && islandHouse.size() <= 1)
  {
    // load the unmodified bottom layer from the island .scp file
    auto islandFile = Island5::islandFileName(island.size, island.fileNumber, island.climate);
    auto path = files->find_path_for_file(islandFile);
    if (path == "")
    {
      throw("cannot find file: " + path);
    }
    std::vector<std::shared_ptr<Chunk>> chunks = Chunk::ReadChunks(path);
    if (chunks[1]->chunk.name == "INSELHAUS")
    {
      finalIslandHouse.push_back(*(std::make_shared<IslandHouse>(chunks[1]->chunk.data, chunks[1]->chunk.length, chunks[1]->chunk.name.c_str())));
    }
    // there is only one islandHouse chunk present, this is the top layer
    if (islandHouse.size() == 1)
    {
      finalIslandHouse.push_back(islandHouse.at(0));
    }
  }
  else
  {
    // the island is modified, first chunk is bottom
    finalIslandHouse.push_back(islandHouse[0]);
    // a possible second chunk is top
    if (islandHouse.size() == 2)
    {
      finalIslandHouse.push_back(islandHouse[1]);
    }
    else
    {
      // create empty top
      IslandHouse i("INSELHAUS");
      finalIslandHouse.push_back(i);
    }
  }
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