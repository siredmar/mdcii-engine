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

#ifndef _ISLAND3_HPP
#define _ISLAND3_HPP

#include <inttypes.h>
#include <map>
#include <string>
#include <vector>

#include "files/files.hpp"

#include "cod/buildings.hpp"

#include "chunk.hpp"
#include "deer.hpp"
#include "island.hpp"
#include "islandhouse.hpp"
#include "military.hpp"
#include "shipyard.hpp"
#include "warehouse.hpp"

struct Island3Data // Insel3
{
    uint8_t inselnr; // ID for this island (per game)
    uint8_t width; // width
    uint8_t height; // height
    uint8_t a; // TODO: unknown
    uint16_t x_pos; // position of island x
    uint16_t y_pos; // position of island y
    uint16_t b; // TODO: unknown
    uint16_t c; // TODO: unknown
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

#endif // _ISLAND3_HPP