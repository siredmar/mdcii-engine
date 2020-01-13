#include <stdlib.h>
#include <inttypes.h>
#include <SDL2/SDL.h>
#include <iostream>
#include <string>
#include <boost/program_options.hpp>

#include "mdcii.hpp"
#include "palette.hpp"
#include "kamera.hpp"
#include "bildspeicher_pal8.hpp"
#include "spielbildschirm.hpp"
#include "cod_parser.hpp"
#include "files.hpp"
#include "files_to_check.hpp"
#include "version.hpp"
#include "mainmenu.hpp"


namespace po = boost::program_options;

class Fps
{
public:
  explicit Fps(int tickInterval = 30)
    : m_tickInterval(tickInterval)
    , m_nextTime(SDL_GetTicks() + tickInterval)
  {
  }

  void next()
  {
    SDL_Delay(getTicksToNextFrame());

    m_nextTime += m_tickInterval;
  }

private:
  const int m_tickInterval;
  Uint32 m_nextTime;

  Uint32 getTicksToNextFrame() const
  {
    Uint32 now = SDL_GetTicks();

    return (m_nextTime <= now) ? 0 : m_nextTime - now;
  }
};

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


Mdcii::Mdcii(int screen_width, int screen_height, bool fullscreen, int rate, const std::string& files_path, const std::string& gam_name)
{
  Anno_version version;
  SDL_Texture* texture;
  SDL_Surface* final_surface;

  auto files = Files::create_instance(files_path);

  version = Version::Detect_game_version();
  if (files->instance()->check_all_files(&files_to_check) == false)
  {
    std::cout << "[ERR] File check failed. Exiting." << std::endl;
    exit(EXIT_FAILURE);
  }

  if (files->instance()->check_file(gam_name) == false)
  {
    std::cout << "[ERR] Could not load savegame: " << gam_name << std::endl;
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
    std::cout << "Could not create window: " << SDL_GetError() << '\n';
    SDL_Quit();
  }

  SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

  SDL_Surface* s8 = SDL_CreateRGBSurface(0, screen_width, screen_height, 8, 0, 0, 0, 0);
  SDL_Color c[256];
  int i, j;
  for (i = 0, j = 0; i < 256; i++)
  {
    c[i].r = palette[j++];
    c[i].g = palette[j++];
    c[i].b = palette[j++];
  }
  SDL_SetPaletteColors(s8->format->palette, c, 0, 255);
  std::ifstream f;
  f.open(gam_name, std::ios_base::in | std::ios_base::binary);
  std::shared_ptr<MainMenu> mainMenu = std::make_shared<MainMenu>(files->instance()->find_path_for_file("basegad.dat"), window, 640, 480);
#if 0
  Fps fps;

  bool quit = false;
  try
  {
    SDL_Event e;
    while (!quit)
    {
      // Handle events on queue
      while (SDL_PollEvent(&e) != 0)
      {
        // User requests quit
        if (e.type == SDL_QUIT)
        {
          quit = true;
        }
        mainMenu->onEvent(e);
      }

      // SDL_SetRenderDrawColor(renderer, 0xd3, 0xd3, 0xd3, 0xff);
      SDL_RenderClear(renderer);

      mainMenu->drawAll();

      // Render the rect to the screen
      SDL_RenderPresent(renderer);

      fps.next();
    }
  }
  catch (const std::runtime_error& e)
  {
    std::string error_msg = std::string("Caught a fatal error: ") + std::string(e.what());
#if defined(_WIN32)
    MessageBoxA(nullptr, error_msg.c_str(), NULL, MB_ICONERROR | MB_OK);
#else
    std::cerr << error_msg << std::endl;
#endif
  }
#else

  std::shared_ptr<Cod_Parser> haeuser_cod = std::make_shared<Cod_Parser>(files->instance()->find_path_for_file("haeuser.cod"), true, false);
  std::shared_ptr<Haeuser> haeuser = std::make_shared<Haeuser>(haeuser_cod);
  Welt welt = Welt(f, haeuser);

  f.close();
  Bildspeicher_pal8 bs(screen_width, screen_height, 0, static_cast<uint8_t*>(s8->pixels), (uint32_t)s8->pitch);

  Spielbildschirm spielbildschirm(bs, haeuser);
  spielbildschirm.zeichne_bild(welt, 0, 0);
  final_surface = SDL_ConvertSurfaceFormat(s8, SDL_PIXELFORMAT_RGB888, 0);
  texture = SDL_CreateTextureFromSurface(renderer, final_surface);
  SDL_RenderClear(renderer);
  SDL_RenderCopy(renderer, texture, NULL, NULL);
  SDL_RenderPresent(renderer);

  if (rate != 0)
  {
    SDL_TimerID timer_id = SDL_AddTimer(1000 / rate, timer_callback, NULL);
  }
  const Uint8* keystate = SDL_GetKeyboardState(NULL);

  SDL_Event e;

  while (1)
  {
    SDL_WaitEvent(&e);
    switch (e.type)
    {
      case SDL_QUIT: exit(EXIT_SUCCESS); break;
      case SDL_USEREVENT:
        int x, y;
        SDL_GetMouseState(&x, &y);

        if (keystate[SDL_SCANCODE_LEFT] || (fullscreen && x == 0))
        {
          spielbildschirm.kamera->nach_links();
        }
        if (keystate[SDL_SCANCODE_RIGHT] || (fullscreen && x == screen_width - 1))
        {
          spielbildschirm.kamera->nach_rechts();
        }
        if (keystate[SDL_SCANCODE_UP] || (fullscreen && y == 0))
        {
          spielbildschirm.kamera->nach_oben();
        }
        if (keystate[SDL_SCANCODE_DOWN] || (fullscreen && y == screen_height - 1))
        {
          spielbildschirm.kamera->nach_unten();
        }

        welt.simulationsschritt();
        spielbildschirm.zeichne_bild(welt, x, y);
        final_surface = SDL_ConvertSurfaceFormat(s8, SDL_PIXELFORMAT_RGB888, 0);
        texture = SDL_CreateTextureFromSurface(renderer, final_surface);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
        break;
      case SDL_KEYDOWN:
        if (e.key.keysym.sym == SDLK_F2)
        {
          spielbildschirm.kamera->setze_vergroesserung(0);
        }
        if (e.key.keysym.sym == SDLK_F3)
        {
          spielbildschirm.kamera->setze_vergroesserung(1);
        }
        if (e.key.keysym.sym == SDLK_F4)
        {
          spielbildschirm.kamera->setze_vergroesserung(2);
        }
        if (e.key.keysym.sym == SDLK_x)
        {
          spielbildschirm.kamera->rechts_drehen();
        }
        if (e.key.keysym.sym == SDLK_y)
        {
          spielbildschirm.kamera->links_drehen();
        }
        if (e.key.keysym.sym == SDLK_ESCAPE)
        {
          exit(EXIT_SUCCESS);
        }
        break;
    }
  }
#endif
}
