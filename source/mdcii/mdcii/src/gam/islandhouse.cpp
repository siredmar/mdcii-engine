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
#include <utility>

#include "gam/islandhouse.hpp"

#include "gam/chunk.hpp"
#include "gam/island.hpp"

IslandHouse::IslandHouse(uint8_t* data, uint32_t length, const char* name, uint16_t width, uint16_t height)
    : name(name)
    , elements(width * height)
    , rawElements(0)
    , width(width)
    , height(height)
{
    size_t islandHouseSize = sizeof(IslandHouseData);
    rawElements = length / islandHouseSize;
    rawIslandHouse = std::shared_ptr<IslandHouseData[]>(new IslandHouseData[rawElements]);
    int element = 0;
    for (unsigned int i = 0; i < length; i += islandHouseSize)
    {
        memcpy((char*)&rawIslandHouse[element++], (data + i), islandHouseSize);
    }
    Finalize();
}

IslandHouse::IslandHouse(const std::string& name, uint16_t width, uint16_t height)
    : name(name)
    , elements(width * height)
    , rawElements(0)
    , width(width)
    , height(height)
{
    Finalize();
}

void IslandHouse::Finalize()
{
    islandHouse = std::shared_ptr<IslandHouseData[]>(new IslandHouseData[elements]);
    auto buildings = Buildings::Instance();
    {
        int y = 0;
        int x = 0;
        for (y = 0; y < height; y++)
        {
            for (x = 0; x < width; x++)
            {
                // Setting default ID meaning 'tile not set'. This gets overwritten later on if on this x,y, position is a valid tile
                islandHouse[y * width + x].id = 0xffff;
            }
        }
    }
    // Now iterate through the passed 'data'. This is the read chunk containing one layer. The layer might contain the bare island
    // or some houses. So it's checked if on the position is a valid field. This step is done to make it easier to calculate
    // graphic indexes for elements bigger than 1,1. The 'posx' and 'posy' fields are used to store the fields partly position if bigger
    // than 1,1 because the position is also given via the array index. So no information is being lost if overwriting 'posx' and 'posy'.
    for (unsigned int i = 0; i < rawElements; i++)
    {
        IslandHouseData& tile = rawIslandHouse[i];

        if (tile.posx >= width || tile.posy >= height)
            continue;

        auto info = buildings->GetHouse(tile.id);
        if (info)
        {
            int elementWidth = 0;
            int elementHeight = 0;
            if (tile.orientation % 2 == 0)
            {
                elementWidth = info.value()->Size.w;
                elementHeight = info.value()->Size.h;
            }
            else
            {
                elementHeight = info.value()->Size.h;
                elementWidth = info.value()->Size.w;
            }
            for (int y = 0; y < elementHeight && tile.posy + y < height; y++)
            {
                for (int x = 0; x < elementWidth && tile.posx + x < width; x++)
                {
                    islandHouse[(tile.posy + y) * width + tile.posx + x] = tile;
                    islandHouse[(tile.posy + y) * width + tile.posx + x].posx = x;
                    islandHouse[(tile.posy + y) * width + tile.posx + x].posy = y;
                }
            }
        }
        else
        {
            islandHouse[tile.posy * width + tile.posx] = tile;
            islandHouse[tile.posy * width + tile.posx].posx = 0;
            islandHouse[tile.posy * width + tile.posx].posy = 0;
        }
    }
}

uint32_t IslandHouse::Size()
{
    return elements;
}

IslandHouseData IslandHouse::Get(uint16_t x, uint16_t y)
{
    return islandHouse[y * width + x];
}

std::shared_ptr<IslandHouseData[]> IslandHouse::GetIslandHouse()
{
    return islandHouse;
}