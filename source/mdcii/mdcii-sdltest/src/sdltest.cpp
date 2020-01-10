
// This file is part of the MDCII Game Engine.
// Copyright (C) 2015  Benedikt Freisen
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

#include <stdlib.h>
#include <inttypes.h>
#include <SDL2/SDL.h>
#include <iostream>
#include <string>
#include <boost/program_options.hpp>

#include "mdcii/mdcii.hpp"

namespace po = boost::program_options;

int main(int argc, char** argv)
{
  // clang-format off
  po::options_description desc("Zulässige Optionen");
  desc.add_options()
    ("width,W", po::value<int>()->default_value(800), "Bildschirmbreite")
    ("height,H", po::value<int>()->default_value(600), "Bildschirmhöhe")
    ("fullscreen,F", po::value<bool>()->default_value(false), "Vollbildmodus (true/false)")
    ("rate,r", po::value<int>()->default_value(10), "Bildrate")
    ("load,l", po::value<std::string>()->default_value("game00.gam"), "Lädt den angegebenen Spielstand (*.gam)")
    ("path,p", po::value<std::string>()->default_value("."), "Pfad zur ANNO1602-Installation")
    ("help,h", "Gibt diesen Hilfetext aus")
  ;
  // clang-format on

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if (vm.count("help"))
  {
    std::cout << desc << std::endl;
    exit(EXIT_SUCCESS);
  }

  Mdcii mdcii(vm["width"].as<int>(), vm["height"].as<int>(), vm["fullscreen"].as<bool>(), vm["rate"].as<int>(), vm["path"].as<std::string>(),
      vm["load"].as<std::string>());
  return 0;
}