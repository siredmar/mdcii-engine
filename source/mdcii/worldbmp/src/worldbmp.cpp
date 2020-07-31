
// This file is part of the MDCII Game Engine.
// Copyright (C) 2020  Armin Schlegel
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
#include <stdlib.h>
#include <string.h>

#include <fstream>
#include <iostream>

#include <boost/program_options.hpp>

#include "mdcii/bsh/bshreader.hpp"
#include "mdcii/cod/cod_parser.hpp"
#include "mdcii/files/files.hpp"
#include "mdcii/framebuffer/framebuffer_pal8.hpp"
#include "mdcii/framebuffer/palette.hpp"
#include "mdcii/gam/island.hpp"
#include "mdcii/version/version.hpp"
#include "mdcii/world/world.hpp"

#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH

namespace po = boost::program_options;

#define XRASTER 16
#define YRASTER 8
#define ELEVATION 10

int main(int argc, char** argv)
{
    // clang-format off
  po::options_description desc("Valid options");
  desc.add_options()
    ("input,i", po::value<std::string>(), "input file (.gam, .szs, .szm)")
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
    Buildings::CreateInstance(std::make_shared<CodParser>(files->FindPathForFile("haeuser.cod"), true, false));
    Palette::CreateInstance(files->FindPathForFile("stadtfld.col"));
    BshReader bshReader(files->FindPathForFile("/gfx/stadtfld.bsh"));

    GamParser gam(vm["input"].as<std::string>(), false);
    World world(gam);
    FramebufferPal8 fb((World::Width + World::Height) * XRASTER, (World::Width + World::Height) * YRASTER, 0);

    TileGraphic water;
    water.index = Buildings::Instance()->GetHouse(1201).value()->Gfx;
    water.groundHeight = 0;

    for (int y = 0; y < World::Height; y++)
    {
        for (int x = 0; x < World::Width; x++)
        {
            auto island = world.IslandOnPosition(x, y);
            TileGraphic gfx = water;
            gfx.index += (y + x * 3) % 12;
            if (island)
            {
                auto tile = island.value()->TerrainTile(x - island.value()->GetIslandData().posx, y - island.value()->GetIslandData().posy);
                gfx = island.value()->GraphicIndexForTile(tile, 0);
                if (gfx.index == -1)
                {
                    gfx = water;
                }
            }
            BshImage& bsh = bshReader.GetBshImage(gfx.index);
            fb.zeichne_bsh_bild_oz(bsh, (x - y + World::Height) * XRASTER, (x + y) * YRASTER + 2 * YRASTER - gfx.groundHeight);
        }
    }

    fb.ExportBMP(vm["output"].as<std::string>().c_str());
}
