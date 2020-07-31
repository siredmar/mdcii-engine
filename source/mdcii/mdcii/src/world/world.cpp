
#include "world/world.hpp"
#include "gam/gam_parser.hpp"
#include "gam/island.hpp"
#include "gam/islandhouse.hpp"

World::World(GamParser gamParser)
    : gamParser(gamParser)
{
}

std::experimental::optional<std::shared_ptr<Island5>> World::IslandOnPosition(uint16_t x, uint16_t y)
{
    for (int i = 0; i < gamParser.Islands5Size(); i++)
    {
        auto island = gamParser.GetIsland5(i);
        auto islandData = island->GetIslandData();
        if ((x >= islandData.posx) && (y >= islandData.posy) && (x < islandData.posx + islandData.width) && (y < islandData.posy + islandData.height))
        {
            return island;
        }
    }
    return {};
}

bool World::IslandNumberOnPosition(uint8_t number, uint16_t x, uint16_t y)
{
    auto i = IslandOnPosition(x, y);
    if (i)
    {
        if (i.value()->GetIslandData().islandNumber == number)
        {
            return true;
        }
    }
    return false;
}

IslandHouseData World::TileOnPosition(uint16_t x, uint16_t y)
{
    auto island = IslandOnPosition(x, y);
    if (island)
    {
        return island.value()->TerrainTile(x, y);
    }
    else
    {
        // todo add sea and animate
        // {
        //   memset(&feld, 0, sizeof(inselfeld_t));
        //   feld.bebauung = 1201;
        //   auto info = buildings->GetHouse(feld.bebauung);
        //   if (info)
        //   {
        //     feld.ani = (0x80000000 + y + x * 3 + ani) % info.value()->AnimAnz;
        //   }
        // }
    }
}
