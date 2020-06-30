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
#include <map>
#include <string>
#include <vector>

#include "../files.hpp"

#include "../cod/haeuser.hpp"

#include "chunk.hpp"
#include "deer.hpp"
#include "islandhouse.hpp"
#include "military.hpp"
#include "shipyard.hpp"
#include "warehouse.hpp"

// ISLAND3 Data

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
  explicit Island3(uint8_t* data, uint32_t length, const std::string& name);
  Island3Data island3;

private:
  std::string name;
};


// ISLAND5 Data

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

enum class Fertility : uint32_t
{
  Random = 0x0000,
  None = 0x1181,
  TobaccoOnly = 0x1183,
  WineOnly = 0x11A1,
  SugarOnly = 0x1189,
  CocoaOnly = 0x11C1,
  WoolOnly = 0x1191,
  SpicesOnly = 0x1185,
  TobaccoAndSpices = 0x1187,
  TobaccoAndSugar = 0x118B,
  SpicesAndSugar = 0x118D,
  WoolAndWine = 0x11B1,
  WoolAndCocoa = 0x11D1,
  WineAndCocoa = 0x11E1
};

enum class IslandModified : uint8_t
{
  False = 0,
  True = 1
};

enum class IslandSize : uint16_t
{
  Little = 0,
  Middle = 1,
  Median = 2,
  Big = 3,
  Large = 4
};

enum class IslandClimate : uint8_t
{
  North = 0,
  South = 1,
  Any = 2
};
static std::map<IslandSize, std::string> islandSizeMap = {
    {IslandSize::Little, "lit"},
    {IslandSize::Middle, "mit"},
    {IslandSize::Median, "med"},
    {IslandSize::Big, "big"},
    {IslandSize::Large, "lar"},
};
static std::map<IslandClimate, std::string> islandClimateMap = {
    {IslandClimate::South, "sued"},
    {IslandClimate::North, "nord"},
    {IslandClimate::Any, "any"},
};

struct TileGraphic
{
  int16_t index;
  uint8_t groundHeight;
};
// ISLAND4 Data
struct Island4Data // Insel5
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
  void setIslandNumber(uint8_t number);
  void setIslandFile(uint16_t fileNumber);
  void addIslandHouse(std::shared_ptr<Chunk> c);
  void addIslandHouse(std::shared_ptr<IslandHouse> i);
  void setDeer(std::shared_ptr<Chunk> c);

  std::vector<std::shared_ptr<IslandHouse>> getIslandHouseData()
  {
    return finalIslandHouse;
  }

  void finalize();

  static IslandClimate randomIslandClimate();
  static std::string islandFileName(IslandSize size, uint8_t islandNumber, IslandClimate climate);
  IslandHouseData TerrainTile(uint8_t x, uint8_t y);
  TileGraphic GraphicIndexForTile(IslandHouseData& tile, uint32_t rotation);
  Island4Data island;

private:
  std::string name;
  Files* files;

  std::vector<std::shared_ptr<IslandHouse>> finalIslandHouse; // INSELHAUS
  std::shared_ptr<IslandHouse> topLayer;
  std::shared_ptr<IslandHouse> bottomLayer;
  Deer deer;                                             // HIRSCH2
  std::vector<std::shared_ptr<IslandHouse>> islandHouse; // INSELHAUS
};


// ISLAND5

struct Island5Data // Insel5
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
  void setIslandNumber(uint8_t number);
  void setIslandFile(uint16_t fileNumber);
  void addIslandHouse(std::shared_ptr<Chunk> c);
  void addIslandHouse(std::shared_ptr<IslandHouse> i);
  void setDeer(std::shared_ptr<Chunk> c);
  Island5Data getIslandData()
  {
    return island;
  }

  std::vector<std::shared_ptr<IslandHouse>> getIslandHouseData()
  {
    return finalIslandHouse;
  }

  void finalize();

  static IslandClimate randomIslandClimate();
  static std::string islandFileName(IslandSize size, uint8_t islandNumber, IslandClimate climate);
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


#endif // _ISLAND_HPP