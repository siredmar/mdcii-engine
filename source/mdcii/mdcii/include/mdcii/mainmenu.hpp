#ifndef MAINMENU_H_
#define MAINMENU_H_

#include <iostream>
#include <memory>
#include "basegad_dat.hpp"
#include "cod_parser.hpp"
#include <SDL2/SDL.h>
#include "sdlgui/window.h"
#include "sdlgui/screen.h"
#include "sdlgui/imageview.h"

using namespace sdlgui;

class MainMenu : public Screen
{
public:
  MainMenu(const std::string& basegad_path, SDL_Window* pwindow, int rwidth, int rheight);

private:
  std::string path;
  std::shared_ptr<Cod_Parser> cod;
  std::shared_ptr<Basegad> basegad;
};
#endif