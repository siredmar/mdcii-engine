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

#ifndef _SCENE_HPP
#define _SCENE_HPP

#include <inttypes.h>
#include <string>

enum class ClimateType : uint8_t
{
  North = 0,
  South = 1,
  Random = 2
};

enum class SizeType : uint8_t
{
  Size0 = 0,
  Size1 = 1,
  Size2 = 2,
  Size3 = 3,
  Size4 = 4
};

enum class NativeFlag : uint8_t
{
  NoNatives = 0,
  Natives = 1
};

enum class GoodsHouseId : uint16_t
{
  None = 0,
  Treasure = 533,
  OreMine = 2401,
  GoldMine = 2405,
  TobaccoPlantation = 1336,
  WinePlantation = 1344,
  SugarPlantation = 1340,
  CocoaPlantation = 1338,
  WoolPlantation = 1332,
  SpicesPlantation = 1342
};

struct RandomGood
{
  GoodsHouseId houseId;
  uint16_t amount; // how many resources
  union {
    uint32_t kind;    // unused?
    uint32_t price;   // unused?
    uint32_t percent; // unused?
  };
};

struct RandomOre
{
  RandomGood smallOreMine; // specifies the random small ore mines
  RandomGood greatOreMine; // specifies the random great ore mines
  RandomGood goldMine;     // specifies the random gold mines
  RandomGood empty;        // unused
};

struct RandomRawMaterials
{
  RandomGood tobacco;  // specifies the distribution for random tobacco 100% growth rates
  RandomGood spices;   // specifies the distribution for random spices 100% growth rates
  RandomGood sugar;    // specifies the distribution for random sugar  100% growth rates
  RandomGood wool;     // specifies the distribution for random wool   100% growth rates
  RandomGood wine;     // specifies the distribution for random wine   100% growth rates
  RandomGood cocoa;    // specifies the distribution for random cocoa  100% growth rates
  RandomGood empty[6]; // empty
};

struct RandomGoodies
{
  RandomGood treasure; // specifies amount of random treasures
  RandomGood empty[3]; // empty
};

struct RandomHardware
{
  RandomGood empty[8]; // unused?
};

struct RandomNativeVillages
{
  uint32_t strawRoofCount; // count of straw roof villages
  uint32_t incasCount;     // count of incas villages
  uint32_t empty[3];       // empty
};

struct Position
{
  uint32_t x; // x position of random island in world
  uint32_t y; // y position of random island in world
};

struct RandomIsland
{
  ClimateType climateType; // specifies the climate for "random" island generation
  SizeType sizenr;         // specifies the size for "random" island generation
  NativeFlag nativflg;     // unused?
  uint8_t islandNumber;    // mixed incremented value with for random islands and the number field from ISLAND5 islands
  uint16_t fileNr;         // 0xFF means this island shall be choosen randomly between all present island files with the given size
  uint16_t empty;          // empty
  Position pos;            // position in the world
};

struct SceneSaveData
{
  char name[64];                       // the name of the scenario
  RandomNativeVillages nativeVillages; // amount of random native villages
  int32_t empty1[3];                   // empty
  int32_t rohstmax;                    // amount of 100% growing raw materials for islands? Always 2?
  int32_t islandsCount;                // overall count of islands
  int32_t goldminsizenr;               // unused? always 1?
  int32_t goldmaxsizenr;               // unused? always 2?
  int32_t empty2;                      // empty
  RandomGood empty3[4];                // empty
  RandomOre ores;                      // random ores
  RandomRawMaterials rawMaterials;     // random raw materials
  RandomGoodies goodies;               // random goodies
  RandomHardware hardware;             // random hardware, unused?
  RandomIsland islands[50];            // random islands definitions
};

class SceneSave
{
public:
  explicit SceneSave(uint8_t* data, uint32_t length, const std::string& name);
  SceneSaveData sceneSave;

private:
  std::string name;
};

struct SceneRankingData
{
  int32_t ranking; // the ranking of the mission 0 to 3 stars
};

class SceneRanking
{
public:
  explicit SceneRanking(uint8_t* data, uint32_t length, const std::string& name);
  SceneRankingData sceneRanking;

private:
  std::string name;
};

#endif // _SCENE_HPP