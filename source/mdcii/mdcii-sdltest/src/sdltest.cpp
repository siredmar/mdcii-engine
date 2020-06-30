
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

#include <SDL2/SDL.h>
#include <boost/program_options.hpp>
#include <inttypes.h>
#include <iostream>
#include <stdlib.h>
#include <string>

#include "mdcii/mdcii.hpp"

namespace po = boost::program_options;

int main(int argc, char** argv)
{
  // clang-format off
  po::options_description desc("Zulässige Optionen");
  desc.add_options()
    ("width,W", po::value<int>()->default_value(1024), "Bildschirmbreite")
    ("height,H", po::value<int>()->default_value(768), "Bildschirmhöhe")
    ("fullscreen,F", po::value<bool>()->default_value(false), "Vollbildmodus (true/false)")
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

  try
  {
    Mdcii mdcii(vm["width"].as<int>(), vm["height"].as<int>(), vm["fullscreen"].as<bool>(), vm["path"].as<std::string>());
  }
  catch (const std::exception& ex)
  {
    std::cout << ex.what() << std::endl;
    exit(1);
  }
  catch (const std::string& ex)
  {
    std::cout << ex << std::endl;
  }
  catch (...)
  {
    std::cout << "unknown exception" << std::endl;
  }

  return 0;
}