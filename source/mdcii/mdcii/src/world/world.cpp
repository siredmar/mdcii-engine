
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
  for (int i = 0; i < gamParser.islands5Size(); i++)
  {
    auto island = gamParser.getIsland5(i);
    auto islandData = island->getIslandData();
    if ((x >= islandData.posx) && (y >= islandData.posy) && (x < islandData.posx + islandData.width) && (y < islandData.posy + islandData.height))
    {
      return island;
    }
  }
  return {};
}

int World::IslandNumberOnPosition(uint16_t x, uint16_t y)
{
  auto i = IslandOnPosition(x, y);
  if (i)
  {
    return i.value()->getIslandData().islandNumber;
  }
  return -1;
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
    //   auto info = haeuser->get_haus(feld.bebauung);
    //   if (info)
    //   {
    //     feld.ani = (0x80000000 + y + x * 3 + ani) % info.value()->AnimAnz;
    //   }
    // }
  }
}