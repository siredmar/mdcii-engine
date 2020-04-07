#ifndef _BSH_TEXTURE
#define _BSH_TEXTURE

#include <SDL2/SDL.h>

#include "bildspeicher_trans_pal8.hpp"
#include "bsh_leser.hpp"
#include "palette.hpp"

class BshImageToSDLTextureConverter
{
public:
  BshImageToSDLTextureConverter(SDL_Renderer* renderer);
  SDL_Texture* Convert(Bsh_bild* image);

private:
  SDL_Renderer* renderer;
};

#endif