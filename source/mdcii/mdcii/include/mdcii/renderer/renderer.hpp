#ifndef __RENDERER_HPP_
#define __RENDERER_HPP_

#include <memory>

#include <SDL2/SDL.h>

#include <experimental/optional>
#include <utility>

#include "../bildspeicher_pal8.hpp"
#include "../cod/haeuser.hpp"
#include "../gam/island.hpp"
#include "../gam/islandhouse.hpp"
#include "../palette.hpp"

class Renderer
{
public:
  Renderer(const int width, const int height, std::shared_ptr<Haeuser> haeuser)
    : palette(Palette::instance())
    , target(SDL_CreateRGBSurface(0, width, height, 8, 0, 0, 0, 0))
    , bs(std::make_shared<Bildspeicher_pal8>(width, height, 0, static_cast<uint8_t*>(target->pixels), (uint32_t)target->pitch))
    , haeuser(haeuser)
  {
    SDL_SetPaletteColors(target->format->palette, palette->getSDLColors(), 0, palette->size());
  }

  void RenderIsland(Island5 i);
  void RenderIsland(std::shared_ptr<Island5> i);

private:
  Palette* palette;
  SDL_Surface* target;
  std::shared_ptr<Bildspeicher_pal8> bs;
  std::shared_ptr<Haeuser> haeuser;


  // std::experimental::optional<IslandHouseData> TerrainTile(uint8_t x, uint8_t y, IslandHouse islandHouseLayer);

  // void grafik_bebauung_inselfeld(feld_t& ziel, IslandHouseData& feld, uint8_t r, std::shared_ptr<Haeuser> haeuser)
  // {
  //   if (feld.bebauung == 0xffff)
  //   {
  //     ziel.index = -1;
  //     ziel.grundhoehe = 0;
  //     return;
  //   }
  //   auto info = haeuser->get_haus(feld.bebauung);

  //   if (!info || info.value()->Gfx == -1)
  //   {
  //     ziel.index = -1;
  //     ziel.grundhoehe = 0;
  //     return;
  //   }
  //   int grafik = info.value()->Gfx;
  //   int index = grafik;
  //   int richtungen = 1;
  //   if (info.value()->Rotate > 0)
  //   {
  //     richtungen = 4;
  //   }
  //   int ani_schritte = 1;
  //   if (info.value()->AnimAnz > 0)
  //   {
  //     ani_schritte = info.value()->AnimAnz;
  //   }
  //   index += info.value()->Rotate * ((r + feld.rot) % richtungen);
  //   switch (feld.rot)
  //   {
  //     case 0:
  //       index += feld.y_pos * info.value()->Size.w + feld.x_pos;
  //       break;
  //     case 1:
  //       index += (info.value()->Size.h - feld.x_pos - 1) * info.value()->Size.w + feld.y_pos;
  //       break;
  //     case 2:
  //       index += (info.value()->Size.h - feld.y_pos - 1) * info.value()->Size.w + (info.value()->Size.w - feld.x_pos - 1);
  //       break;
  //     case 3:
  //       index += feld.x_pos * info.value()->Size.w + (info.value()->Size.w - feld.y_pos - 1);
  //       break;
  //     default:
  //       break;
  //   }
  //   index += info.value()->Size.h * info.value()->Size.w * richtungen * (feld.ani % ani_schritte);
  //   ziel.index = index;
  //   int grundhoehe = 0;
  //   if (info.value()->Posoffs == 20)
  //   {
  //     grundhoehe = 1;
  //   }
  //   ziel.grundhoehe = grundhoehe;
  // }

  // void Insel::grafik_bebauung(feld_t& ziel, uint8_t x, uint8_t y, uint8_t r)
  // {
  //   inselfeld_t feld;
  //   inselfeld_bebauung(feld, x, y);
  //   grafik_bebauung_inselfeld(ziel, feld, r, haeuser);
  // }
};

#endif // __RENDERER_HPP_