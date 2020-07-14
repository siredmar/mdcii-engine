
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
#include <errno.h>
#include <inttypes.h>
#include <iostream>
#include <stdlib.h>

#include <fstream>
#include <string>

#include <boost/program_options.hpp>

#include "mdcii/bsh/bshreader.hpp"
#include "mdcii/cod/cod_parser.hpp"
#include "mdcii/files/files.hpp"
#include "mdcii/framebuffer/framebuffer_pal8.hpp"
#include "mdcii/framebuffer/palette.hpp"
#include "mdcii/gam/gam_parser.hpp"
#include "mdcii/insel.hpp"

namespace po = boost::program_options;

#define XRASTER 32
#define YRASTER 16
#define ELEVATION 20

/*
  Must be run from within the anno1602 folder
  usage: ./islandbmp <island.scp> <output.bmp> <anno path>
*/
int main(int argc, char* argv[])
{
  // clang-format off
  po::options_description desc("Valid options");
  desc.add_options()
    ("island,i", po::value<std::string>(), "input file (.scp)")
    ("output,o", po::value<std::string>(), "output file (.bmp)")
    ("path,p", po::value<std::string>()->default_value("."), "1602 AD installation path")
    ("help,h", "Print this help");
  // clang-format on
  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if (vm.count("help"))
  {
    std::cout << desc << std::endl;
    exit(EXIT_SUCCESS);
  }

  auto files = Files::CreateInstance(vm["path"].as<std::string>());
  std::shared_ptr<CodParser> buildingsCod = std::make_shared<CodParser>(files->Instance()->FindPathForFile("haeuser.cod"), true, false);
  (void)Buildings(buildingsCod);
  Palette::CreateInstance(files->Instance()->FindPathForFile("stadtfld.col"));
  BshReader bshReader(files->Instance()->FindPathForFile("/gfx/stadtfld.bsh"));

  GamParser gam(vm["island"].as<std::string>(), false);

  int selectedIsland = -1;
  if (gam.Islands5Size() > 1)
  {
    std::cout << "Islands: " << static_cast<int>(gam.Islands5Size()) << std::endl;
    while (selectedIsland > static_cast<int>(gam.Islands5Size() - 1) || selectedIsland == -1)
    {
      std::cout << "Select island to export: " << std::endl;
      std::cin >> selectedIsland;
    }
  }
  else
  {
    selectedIsland = 0;
  }

  auto island = gam.GetIsland5(selectedIsland);
  uint8_t width = island->GetIslandData().width;
  uint8_t height = island->GetIslandData().height;

  FramebufferPal8 fb((width + height) * XRASTER, (width + height) * YRASTER, 0);

  int x, y;
  for (y = 0; y < height; y++)
  {
    for (x = 0; x < width; x++)
    {
      auto tile = island->TerrainTile(x, y);
      auto gfx = island->GraphicIndexForTile(tile, 0);
      if (gfx.index != -1)
      {
        BshImage& bsh = bshReader.GetBshImage(gfx.index);
        fb.zeichne_bsh_bild_oz(bsh, (x - y + height) * XRASTER, (x + y) * YRASTER + 2 * YRASTER - gfx.groundHeight);
      }
    }
  }
  fb.ExportBMP(vm["output"].as<std::string>().c_str());
  return 0;
}
