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

struct OreMountainData // Erzberg
{
    uint8_t ware; // which good lays here?
    uint8_t posx; // position on island x
    uint8_t posy; // position on island y
    uint8_t playerflags; // which player knows this secret? (DANGER: PLAYER_MAX)
    uint8_t kind; // which kind?
    uint8_t empty; // empty
    uint16_t stock; // how much of this good lays here?
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
    { IslandSize::Little, "lit" },
    { IslandSize::Middle, "mit" },
    { IslandSize::Median, "med" },
    { IslandSize::Big, "big" },
    { IslandSize::Large, "lar" },
};
static std::map<IslandClimate, std::string> islandClimateMap = {
    { IslandClimate::South, "sued" },
    { IslandClimate::North, "nord" },
    { IslandClimate::Any, "any" },
};

struct TileGraphic
{
    int16_t index;
    uint8_t groundHeight;
};

#include "island3.hpp"
#include "island4.hpp"
#include "island5.hpp"

#endif // _ISLAND_HPP