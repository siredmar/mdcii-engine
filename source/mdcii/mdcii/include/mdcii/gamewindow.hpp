#ifndef GAMEWINDOW_H_
#define GAMEWINDOW_H_

#include <iostream>
#include <memory>
#include "haeuser.hpp"
#include "cod_parser.hpp"
#include "files.hpp"

#include <SDL2/SDL.h>
#include "sdlgui/window.h"
#include "sdlgui/screen.h"
#include "sdlgui/imageview.h"

using namespace sdlgui;

class GameWindow : public Screen
{
public:
  GameWindow(
      SDL_Renderer* renderer, const std::string& haeuser_cod, SDL_Window* pwindow, int rwidth, int rheight, const std::string& gam_name, bool fullscreen);
  void Handle();

private:
  SDL_Renderer* renderer;
  std::shared_ptr<Cod_Parser> cod;
  std::shared_ptr<Haeuser> haeuser;
  int width;
  int height;
  std::string gam_name;
  bool fullscreen;
};
#endif