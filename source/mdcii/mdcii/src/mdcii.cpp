#include <inttypes.h>
#include <iostream>
#include <stdlib.h>
#include <string>

#include <boost/program_options.hpp>

#include <SDL2/SDL.h>

#include "bildspeicher_pal8.hpp"
#include "cod_parser.hpp"
#include "files.hpp"
#include "files_to_check.hpp"
#include "fps.hpp"
#include "gam/gamParser.hpp"
#include "gamewindow.hpp"
#include "kamera.hpp"
#include "mainmenu.hpp"
#include "mdcii.hpp"
#include "palette.hpp"
#include "spielbildschirm.hpp"
#include "version.hpp"


namespace po = boost::program_options;

Uint32 Mdcii::timer_callback(Uint32 interval, void* param)
{
  SDL_Event event;
  SDL_UserEvent userevent;

  // push an SDL_USEREVENT event into the queue

  userevent.type = SDL_USEREVENT;
  userevent.code = 0;
  userevent.data1 = NULL;
  userevent.data2 = NULL;

  event.type = SDL_USEREVENT;
  event.user = userevent;

  SDL_PushEvent(&event);
  return (interval);
}


Mdcii::Mdcii(int screen_width, int screen_height, bool fullscreen, int rate, const std::string& files_path)
{
  Anno_version version;

  auto files = Files::create_instance(files_path);

  // try
  // {
  //   GamParser gamParser("buendnis.szs");
  // }
  // catch (std::exception& ex)
  // {
  //   std::cout << ex.what() << std::endl;
  // }

  version = Version::Detect_game_version();
  if (files->instance()->check_all_files(&files_to_check) == false)
  {
    std::cout << "[ERR] File check failed. Exiting." << std::endl;
    exit(EXIT_FAILURE);
  }

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0)
  {
    exit(EXIT_FAILURE);
  }
  atexit(SDL_Quit);

  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_Window* window = SDL_CreateWindow("mdcii-sdltest", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screen_width, screen_height,
      (fullscreen ? SDL_WINDOW_FULLSCREEN : 0) | SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_SHOWN);

  if (window == NULL)
  {
    // In the event that the window could not be made...
    std::cout << "[ERR] Could not create window: " << SDL_GetError() << '\n';
    SDL_Quit();
  }

  SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
  // SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

  auto palette = Palette::create_instance(files->instance()->find_path_for_file("stadtfld.col"));
  auto haeuser_cod = std::make_shared<Cod_Parser>(files->instance()->find_path_for_file("haeuser.cod"), true, false);
  auto haeuser = std::make_shared<Haeuser>(haeuser_cod);
  auto basegad_dat = std::make_shared<Cod_Parser>(files->instance()->find_path_for_file("base.gad"), false, false);
  auto basegad = std::make_shared<Basegad>(basegad_dat);

  // GameWindow gameWindow(renderer, haeuser, window, screen_width, screen_height, gam_name, fullscreen);
  // gameWindow.Handle();

  MainMenu mainMenu(renderer, basegad, window, screen_width, screen_height, fullscreen);
  mainMenu.Handle();
}
