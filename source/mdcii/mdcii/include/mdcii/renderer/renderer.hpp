#ifndef __RENDERER_HPP_
#define __RENDERER_HPP_

#include <memory>

#include <SDL2/SDL.h>

#include <experimental/optional>
#include <utility>

#include "cod/haeuser.hpp"
#include "framebuffer/framebuffer_pal8.hpp"
#include "framebuffer/palette.hpp"
#include "gam/island.hpp"
#include "gam/islandhouse.hpp"

class Renderer
{
public:
  Renderer(const int width, const int height)
    : palette(Palette::Instance())
    , target(SDL_CreateRGBSurface(0, width, height, 8, 0, 0, 0, 0))
    , fb(std::make_shared<FramebufferPal8>(width, height, 0, static_cast<uint8_t*>(target->pixels), (uint32_t)target->pitch))
    , buildings(Buildings::Instance())
  {
    SDL_SetPaletteColors(target->format->palette, palette->GetSDLColors(), 0, palette->size());
  }

  void RenderIsland(Island5 i);
  void RenderIsland(std::shared_ptr<Island5> i);

private:
  Palette* palette;
  SDL_Surface* target;
  std::shared_ptr<FramebufferPal8> fb;
  std::shared_ptr<Buildings> buildings;


  // std::experimental::optional<IslandHouseData> TerrainTile(uint8_t x, uint8_t y, IslandHouse islandHouseLayer);

  // void grafik_bebauung_inselfeld(feld_t& target, IslandHouseData& feld, uint8_t r, std::shared_ptr<Haeuser> buildings)
  // {
  //   if (feld.bebauung == 0xffff)
  //   {
  //     target.index = -1;
  //     target.grundhoehe = 0;
  //     return;
  //   }
  //   auto info = buildings->GetHouse(feld.bebauung);

  //   if (!info || info.value()->Gfx == -1)
  //   {
  //     target.index = -1;
  //     target.grundhoehe = 0;
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
  //   target.index = index;
  //   int grundhoehe = 0;
  //   if (info.value()->Posoffs == 20)
  //   {
  //     grundhoehe = 1;
  //   }
  //   target.grundhoehe = grundhoehe;
  // }

  // void Insel::grafik_bebauung(feld_t& target, uint8_t x, uint8_t y, uint8_t r)
  // {
  //   inselfeld_t feld;
  //   inselfeld_bebauung(feld, x, y);
  //   grafik_bebauung_inselfeld(target, feld, r, buildings);
  // }
};

#endif // __RENDERER_HPP_