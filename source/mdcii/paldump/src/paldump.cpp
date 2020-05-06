// This file is part of the MDCII Game Engine.
// Copyright (C) 2020 Armin Schlegel
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include <boost/program_options.hpp>
#include <iostream>

#include "mdcii/palette.hpp"

namespace po = boost::program_options;

using namespace std;

int main(int argc, char** argv)
{
  string input_name;

  // clang-format off
  po::options_description desc("Options");
  desc.add_options()
    ("input,i", po::value<string>(&input_name), "Input palette file (*.col)")
    ("output,o", po::value<string>(&input_name), "Output file (*.bmp)")
    ("help,h", "This help text")
  ;
  // clang-format on

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if (vm.count("help"))
  {
    cout << desc << endl;
    exit(EXIT_SUCCESS);
  }

  if (vm.count("input") != 1)
  {
    cout << "Keine Eingabedatei angegeben" << endl;
    exit(EXIT_FAILURE);
  }

  auto palette = Palette::create_instance(input_name);

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0)
  {
    exit(EXIT_FAILURE);
  }
  atexit(SDL_Quit);

  SDL_Window* window = SDL_CreateWindow(
      "mdcii-sdltest", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 640, SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_SHOWN);

  if (window == NULL)
  {
    // In the event that the window could not be made...
    std::cout << "[ERR] Could not create window: " << SDL_GetError() << '\n';
    SDL_Quit();
  }

  SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
  SDL_RenderClear(renderer);
  int x = 0;
  int y = -40;
  for (int i = 0; i < palette->size(); i++)
  {
    if (i % 16 == 0)
    {
      y += 40;
      x = 0;
    }
    else
    {
      x += 40;
    }
    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = 40;
    rect.h = 40;
    SDL_SetRenderDrawColor(renderer, palette->getColor(i).getRed(), palette->getColor(i).getGreen(), palette->getColor(i).getBlue(), 0);
    SDL_RenderFillRect(renderer, &rect);
  }
  SDL_RenderPresent(renderer);
  cin.get();
}
