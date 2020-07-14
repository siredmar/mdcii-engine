#include "renderer/renderer.hpp"

void Renderer::RenderIsland(Island5 i)
{
  auto islandHouseLayers = i.GetIslandHouseData();
  auto islandHouse = islandHouseLayers[0]->GetIslandHouse();
  // for (auto tile : islandHouse)
  // {
  //   auto element = buildings->GetHouse(tile.id);
  //   if (element)
  //   {
  //     // int x = tile.posx;
  //     // int y = tile.posy;
  //     // BshImage& bsh = bshReader.GetBshImage(element.value()->Gfx);
  //     // int elevation = element.value()->Posoffs;
  //     // fb.zeichne_bsh_bild_oz(bsh, (x - y + height) * XRASTER, (x + y) * YRASTER + 2 * YRASTER - elevation);
  //   }
  // }
}

void Renderer::RenderIsland(std::shared_ptr<Island5> i)
{
  return RenderIsland(*i.get());
}

// std::experimental::optional<IslandHouseData> Renderer::TerrainTile(uint8_t x, uint8_t y)
// {
//   auto h = islandHouseLayer.GetMap(std::make_pair(x, y));
//   if (h)
//   {
//     uint8_t xp = h.value().posx;
//     uint8_t yp = h.value().posy;
//     if ((yp > y) || (xp > x))
//     {
//       // TODO "target" auf einen sinnvollen Wert setzen
//       return {};
//     }
//     auto r = islandHouseLayer.GetMap(std::make_pair(y - yp, x - xp));
//     if (r)
//     {
//       IslandHouseData ret = r.value();
//       ret.posx = xp;
//       ret.posy = yp;
//       return ret;
//     }
//     return {};
//   }
//   return {};
// }