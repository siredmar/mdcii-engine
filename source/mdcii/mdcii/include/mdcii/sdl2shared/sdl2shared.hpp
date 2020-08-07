#ifndef _SDL2SHARED_HPP_
#define _SDL2SHARED_HPP_

#include <SDL2/SDL.h>
#include <memory> // shared_ptr

namespace sdl2
{
static void SDL_DelRes(SDL_Window* r)
{
    SDL_DestroyWindow(r);
}
static void SDL_DelRes(SDL_Renderer* r)
{
    SDL_DestroyRenderer(r);
}
static void SDL_DelRes(SDL_Texture* r)
{
    SDL_DestroyTexture(r);
}
static void SDL_DelRes(SDL_Surface* r)
{
    SDL_FreeSurface(r);
}

template <typename T>
std::shared_ptr<T> make_shared(T* t)
{
    return std::shared_ptr<T>(t, [](T* t) { SDL_DelRes(t); });
}
}
#endif // _SDL2SHARED_HPP_