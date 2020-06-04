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

#ifndef _RAWGROWTH_HPP
#define _RAWGROWTH_HPP

#include <inttypes.h>
#include <string>
#include <vector>

struct RawGrowthData // ROHWACHS2
{
  uint8_t islandNumber;   // On which island is the raw good
  uint8_t posx;           // Position on the island
  uint8_t posy;           // Position on the island
  uint8_t speed;          // Which speed counter (MAXWACHSSPEEDKIND)
  uint8_t speedcnt;       // If (Timecnt != SpeedCnt) currentfield->animationCount++ (MAXROHWACHSCNT)
  uint8_t animationCount; // current animation index
};

class RawGrowth
{
public:
  RawGrowth()
  {
  }
  explicit RawGrowth(uint8_t* data, uint32_t length, const std::string& name);
  std::vector<RawGrowthData> rawGrowth;

private:
  std::string name;
};

#endif // _RAWGROWTH_HPP