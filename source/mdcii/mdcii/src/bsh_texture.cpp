#include "bsh_texture.hpp"
#include <ios>
#include <iostream>

BshImageToSDLTextureConverter::BshImageToSDLTextureConverter(SDL_Renderer* renderer)
  : renderer(renderer)
{
}

SDL_Texture* BshImageToSDLTextureConverter::Convert(Bsh_bild* image)
{
  auto palette = Palette::instance();
  SDL_Surface* final_surface;
  SDL_Surface* s8 = SDL_CreateRGBSurface(0, image->breite, image->hoehe, 8, 0, 0, 0, 0);
  SDL_Color c[palette->size()];
  int i, j;
  for (i = 0, j = 0; i < palette->size(); i++)
  {
    c[i].r = palette->getColor(i).getRed();
    c[i].g = palette->getColor(i).getGreen();
    c[i].b = palette->getColor(i).getBlue();
  }
  SDL_SetPaletteColors(s8->format->palette, c, 0, palette->size());
  Bildspeicher_trans_pal8 bs(image->breite, image->hoehe, 0, static_cast<uint8_t*>(s8->pixels), (uint32_t)s8->pitch, palette->getTransparentColor());
  bs.bild_loeschen();
  bs.zeichne_bsh_bild(*image, 0, 0);
  auto transparentColor = palette->getColor(palette->getTransparentColor());
  SDL_SetColorKey(s8, SDL_TRUE, SDL_MapRGB(s8->format, transparentColor.getRed(), transparentColor.getGreen(), transparentColor.getBlue()));
  std::cout << std::hex << (int)*((uint8_t*)s8->pixels) << ", " << std::hex << (int)*((uint8_t*)(s8->pixels + 1)) << ", " << std::hex
            << (int)*((uint8_t*)(s8->pixels + 2)) << std::endl;
  // SDL_SetColorKey(s8, SDL_TRUE, SDL_MapRGB(s8->format, palette->getColor(25).getRed(), palette->getColor(25).getGreen(), palette->getColor(25).getBlue()));
  final_surface = SDL_ConvertSurfaceFormat(s8, SDL_PIXELFORMAT_RGB888, 0);
  auto texture = SDL_CreateTextureFromSurface(renderer, final_surface);
  return texture;
}