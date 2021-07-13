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
    , files(Files::Instance())
{
    memcpy((char*)&island, data, length);
}

Island5::Island5(const Island3& island3)
{
    island.islandNumber = island3.island3.inselnr;
    island.posx = island3.island3.x_pos;
    island.posy = island3.island3.y_pos;
    island.width = island3.island3.width;
    island.height = island3.island3.height;
}

Island5::Island5(const Island4& island4)
{
    memcpy(&island, &island4.island, sizeof(Island4Data));
    island.fileNumber = island4.island.fileNumber;
    island.modifiedFlag = IslandModified::True;
    island.islandNumber = 0;
}

void Island5::SetIslandNumber(uint8_t number)
{
    island.islandNumber = number;
}

void Island5::SetPosition(uint16_t x, uint16_t y)
{
    island.posx = x;
    island.posy = y;
}

void Island5::SetIslandFile(uint16_t fileNumber)
{
    island.fileNumber = fileNumber;
}

void Island5::AddIslandHouse(std::shared_ptr<Chunk> c)
{
    islandHouse.push_back(std::make_shared<IslandHouse>(c->chunk.data, c->chunk.length, c->chunk.name.c_str(), island.width, island.height));
}

void Island5::AddIslandHouse(std::shared_ptr<IslandHouse> i)
{
    islandHouse.push_back(i);
}

std::string Island5::IslandFileName(IslandSize size, uint8_t islandNumber, IslandClimate climate)
{
    return boost::str(boost::format("%s/%s%02d.scp") % islandClimateMap[climate] % islandSizeMap[size] % (int)islandNumber);
}

IslandClimate Island5::RandomIslandClimate()
{
    std::time_t now = std::time(0);
    boost::random::mt19937 gen{ static_cast<std::uint32_t>(now) };
    return static_cast<IslandClimate>(gen() % static_cast<uint32_t>(IslandClimate::Any));
}

void Island5::Finalize()
{
    // island is unmodified, load bottom islandHouse from island file
    if (island.modifiedFlag == IslandModified::False && islandHouse.size() <= 1)
    {
        // load the unmodified bottom layer from the island .scp file
        auto islandFile = Island5::IslandFileName(island.size, island.fileNumber, island.climate);
        auto path = files->FindPathForFile(islandFile);
        if (path == "")
        {
            throw("[EER] cannot find file: " + path);
        }
        std::vector<std::shared_ptr<Chunk>> chunks = Chunk::ReadChunks(path);
        if (chunks[1]->chunk.name == "INSELHAUS")
        {
            auto i = std::make_shared<IslandHouse>(chunks[1]->chunk.data, chunks[1]->chunk.length, chunks[1]->chunk.name.c_str(), island.width, island.height);
            islandHouse.insert(islandHouse.begin(), i);
        }
        if (islandHouse.size() == 2)
        {
            finalIslandHouse.push_back(islandHouse[0]);
            finalIslandHouse.push_back(islandHouse[1]);
        }
        // there is only one islandHouse chunk present, this is the bot layer
        if (islandHouse.size() == 1)
        {
            finalIslandHouse.push_back(islandHouse[0]);
            // create empty top
            auto empty = std::make_shared<IslandHouse>("INSELHAUS", island.width, island.height);
            finalIslandHouse.push_back(empty);
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
            auto islandHouse = std::make_shared<IslandHouse>("INSELHAUS", island.width, island.height);
            finalIslandHouse.push_back(islandHouse);
        }
    }
    bottomLayer = finalIslandHouse[0];
    topLayer = finalIslandHouse[1];
}

void Island5::SetDeer(std::shared_ptr<Chunk> c)
{
    deer = Deer(c->chunk.data, c->chunk.length, "HIRSCH2");
}

IslandHouseData Island5::TerrainTile(uint8_t x, uint8_t y)
{
    // Check if top layer has something for the given position, fallback to bottom layer
    auto h = topLayer->Get(x, y);
    if (h.id == 0xFFFF)
    {
        h = bottomLayer->Get(x, y);
    }

    uint8_t xp = h.posx;
    uint8_t yp = h.posy;
    if ((yp > y) || (xp > x))
    {
        return h;
    }

    h = topLayer->Get(x - xp, y - yp);
    if (h.id == 0xFFFF)
    {
        h = bottomLayer->Get(x - xp, y - yp);
    }
    h.posx = xp;
    h.posy = yp;
    return h;
}

TileGraphic Island5::GraphicIndexForTile(IslandHouseData& tile, uint32_t rotation)
{
    auto buildings = Buildings::Instance();
    TileGraphic target;
    if (tile.id == 0xFFFF)
    {
        target.index = -1;
        target.groundHeight = 0;
        return target;
    }
    auto info = buildings->GetHouse(tile.id);

    if (!info || info.value()->Gfx == -1)
    {
        target.index = -1;
        target.groundHeight = 0;
        return target;
    }
    int index = info.value()->Gfx;
    int directions = 1;
    if (info.value()->Rotate > 0)
    {
        directions = 4;
    }
    int aniSteps = 1;
    if (info.value()->AnimAnz > 0)
    {
        aniSteps = info.value()->AnimAnz;
    }
    index += info.value()->Rotate * ((rotation + tile.orientation) % directions);
    switch (tile.orientation)
    {
        case 0:
            index += tile.posy * info.value()->Size.w + tile.posx;
            break;
        case 1:
            index += (info.value()->Size.h - tile.posx - 1) * info.value()->Size.w + tile.posy;
            break;
        case 2:
            index += (info.value()->Size.h - tile.posy - 1) * info.value()->Size.w + (info.value()->Size.w - tile.posx - 1);
            break;
        case 3:
            index += tile.posx * info.value()->Size.w + (info.value()->Size.w - tile.posy - 1);
            break;
        default:
            break;
    }
    index += info.value()->Size.h * info.value()->Size.w * directions * (tile.animationCount % aniSteps);
    target.index = index;
    target.groundHeight = 0;
    if (info.value()->Posoffs == 20)
    {
        target.groundHeight = info.value()->Posoffs;
    }
    return target;
}
