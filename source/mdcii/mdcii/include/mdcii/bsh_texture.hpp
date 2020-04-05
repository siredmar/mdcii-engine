#ifndef _BSH_TEXTURE
#define _BSH_TEXTURE

#include <SDL2/SDL.h>

#include <bildspeicher_pal8.hpp>
#include <bsh_leser.hpp>
#include <palette.hpp>

class BshImageToSDLTextureConverter
{
public:
  BshImageToSDLTextureConverter(SDL_Renderer* renderer)
    : renderer(renderer)
  {
  }

  SDL_Texture* Convert(Bsh_bild* image)
  {
    SDL_Surface* final_surface;
    SDL_Surface* s8 = SDL_CreateRGBSurface(0, image->breite, image->hoehe, 8, 0, 0, 0, 0);
    SDL_Color c[256];
    int i, j;
    for (i = 0, j = 0; i < 256; i++)
    {
      c[i].r = palette[j++];
      c[i].g = palette[j++];
      c[i].b = palette[j++];
    }
    SDL_SetPaletteColors(s8->format->palette, c, 0, 255);
    SDL_SetColorKey(s8, SDL_TRUE, SDL_MapRGB(s8->format, 0xFF, 0, 0xFF));
    Bildspeicher_pal8 bs(image->breite, image->hoehe, 0, static_cast<uint8_t*>(s8->pixels), (uint32_t)s8->pitch);
    bs.bild_loeschen();
    bs.zeichne_bsh_bild(*image, 0, 0);
    final_surface = SDL_ConvertSurfaceFormat(s8, SDL_PIXELFORMAT_RGB888, 0);
    auto texture = SDL_CreateTextureFromSurface(renderer, final_surface);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    return texture;
  }

private:
  SDL_Renderer* renderer;
};

#endif