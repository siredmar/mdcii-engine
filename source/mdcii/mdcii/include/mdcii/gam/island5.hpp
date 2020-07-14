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

#ifndef _ISLAND5_HPP
#define _ISLAND5_HPP

#include <inttypes.h>
#include <map>
#include <string>
#include <vector>

#include "files/files.hpp"

#include "cod/haeuser.hpp"

#include "chunk.hpp"
#include "deer.hpp"
#include "island.hpp"
#include "islandhouse.hpp"
#include "military.hpp"
#include "shipyard.hpp"
#include "warehouse.hpp"

struct Island5Data
{
  uint8_t islandNumber;
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
  uint16_t fileNumber;
  IslandSize size;
  IslandClimate climate;
  IslandModified modifiedFlag; // flag that indicates if the island is `original` (empty INSELHAUS chunk) or `modified` (filled INSELHAUS chunk).
  uint8_t duerrproz;
  uint8_t rotier;
  uint32_t seeplayerflags;
  uint32_t duerrcnt;
  uint32_t leer3;
};

class Island5
{
public:
  Island5()
  {
  }
  explicit Island5(uint8_t* data, uint32_t length, const std::string& name);
  explicit Island5(const Island3& island3);
  explicit Island5(const Island4& island4);
  void SetIslandNumber(uint8_t number);
  void SetIslandFile(uint16_t fileNumber);
  void AddIslandHouse(std::shared_ptr<Chunk> c);
  void AddIslandHouse(std::shared_ptr<IslandHouse> i);
  void SetDeer(std::shared_ptr<Chunk> c);
  Island5Data GetIslandData()
  {
    return island;
  }

  std::vector<std::shared_ptr<IslandHouse>> GetIslandHouseData()
  {
    return finalIslandHouse;
  }

  void Finalize();

  static IslandClimate RandomIslandClimate();
  static std::string IslandFileName(IslandSize size, uint8_t islandNumber, IslandClimate climate);
  IslandHouseData TerrainTile(uint8_t x, uint8_t y);
  TileGraphic GraphicIndexForTile(IslandHouseData& tile, uint32_t rotation);

private:
  std::string name;
  Files* files;

  Island5Data island;
  std::vector<std::shared_ptr<IslandHouse>> finalIslandHouse; // INSELHAUS
  std::shared_ptr<IslandHouse> topLayer;
  std::shared_ptr<IslandHouse> bottomLayer;
  Deer deer;                                             // HIRSCH2
  std::vector<std::shared_ptr<IslandHouse>> islandHouse; // INSELHAUS
};


#endif // _ISLAND5_HPP