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

#include "kontor.hpp"

#define MAXSIZEKIND 5

enum class ClimateType : uint8_t
{
  North = 0,
  South = 1,
  Random = 2,
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

struct RandomIslands
{
  ClimateType climateType; // specifies the climate for "random" island generation
  SizeType sizenr;         // specifies the size for "random" island generation
  NativeFlag nativflg : 1; // unused?
  uint8_t islandNumber;    // mixed increment with this number and the number field from ISLAND5 islands
  uint16_t filenr;         // 0xFF means this island shall be choosen randomly between all present island files with the given size
  uint16_t empty;          // empty
  uint32_t posx;           // position
  uint32_t posy;           // position
};

struct SceneSaveData
{
  uint8_t name[64]; // the name of the scenario
  int32_t nativanz[5];
  int32_t empty1[3];
  int32_t rohstmax;
  int32_t inselanz;
  int32_t goldminsizenr;
  int32_t goldmaxsizenr;
  int32_t empty2; // unused?
  KontorWare empty3[4];
  KontorWare erze[4];
  KontorWare rohst[12];
  KontorWare goodies[4];
  KontorWare handware[8];
  RandomIslands insel[50];
};

class SceneSave
{
public:
  SceneSave(uint8_t* data, uint32_t length, const std::string& name);
  SceneSaveData sceneSave;

private:
  std::string name;
};

struct SceneRankingData
{
  int32_t ranking; // The ranking of the mission 0 to 3 stars
};

class SceneRanking
{
public:
  SceneRanking(uint8_t* data, uint32_t length, const std::string& name);
  SceneRankingData sceneRanking;

private:
  std::string name;
};

#endif // _SCENE_HPP