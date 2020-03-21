#ifndef GAMEWINDOW_H_
#define GAMEWINDOW_H_

#include <iostream>
#include <memory>
#include "basegad_dat.hpp"
#include "cod_parser.hpp"
#include <SDL2/SDL.h>
#include "sdlgui/window.h"
#include "sdlgui/screen.h"
#include "sdlgui/imageview.h"

using namespace sdlgui;

class GameWindow : public Screen
{
public:
  GameWindow(const std::string& basegad_path, SDL_Window* pwindow, int rwidth, int rheight, SDL_Texture* texture);

private:
  std::string path;
  std::shared_ptr<Cod_Parser> cod;
  std::shared_ptr<Basegad> basegad;
};
#endif