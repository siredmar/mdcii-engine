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
  SDL_SetPaletteColors(s8->format->palette, palette->getSDLColors(), 0, palette->size());
  Bildspeicher_trans_pal8 bs(image->breite, image->hoehe, 0, static_cast<uint8_t*>(s8->pixels), (uint32_t)s8->pitch, palette->getTransparentColor());
  bs.bild_loeschen();
  bs.zeichne_bsh_bild(*image, 0, 0);
  auto transparentColor = palette->getColor(palette->getTransparentColor());
  final_surface = SDL_ConvertSurfaceFormat(s8, SDL_PIXELFORMAT_RGB888, 0);
  SDL_SetColorKey(
      final_surface, SDL_TRUE, SDL_MapRGB(final_surface->format, transparentColor.getRed(), transparentColor.getGreen(), transparentColor.getBlue()));
  auto texture = SDL_CreateTextureFromSurface(renderer, final_surface);
  return texture;
}