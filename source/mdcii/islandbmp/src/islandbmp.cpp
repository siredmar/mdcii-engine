
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

#include "mdcii/bildspeicher_pal8.hpp"
#include "mdcii/bsh_leser.hpp"
#include "mdcii/cod/cod_parser.hpp"
#include "mdcii/files.hpp"
#include "mdcii/gam/gam_parser.hpp"
#include "mdcii/insel.hpp"
#include "mdcii/palette.hpp"
#

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
  po::options_description desc("Zulässige Optionen");
  desc.add_options()
    ("island,i", po::value<std::string>(), "input file (.scp)")
    ("output,o", po::value<std::string>(), "output file (.bmp)")
    ("path,p", po::value<std::string>()->default_value("."), "1602 AD installation path")
    ("help,h", "Print this help")
  ;
  // clang-format on
  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);


  auto files = Files::create_instance(vm["path"].as<std::string>());
  std::shared_ptr<Cod_Parser> haeuser_cod = std::make_shared<Cod_Parser>(files->instance()->find_path_for_file("haeuser.cod"), true, false);
  std::shared_ptr<Haeuser> haeuser = std::make_shared<Haeuser>(haeuser_cod);
  Palette::create_instance(files->instance()->find_path_for_file("stadtfld.col"));
  Bsh_leser bshReader(files->instance()->find_path_for_file("gfx/stadtfld.bsh"));

  GamParser gam(vm["island"].as<std::string>(), false);

  // Insel insel = Insel(&inselX, &inselhaus, haeuser);
  if (gam.islands5Size() == 1)
  {
    auto i = gam.getIsland5(0);
    uint8_t width = i->getIslandData().width;
    uint8_t height = i->getIslandData().height;
  }


  // Bildspeicher_pal8 bs((width + height) * XRASTER, (width + height) * YRASTER, 0);

  // int x, y;
  // for (y = 0; y < height; y++)
  // {
  //   for (x = 0; x < width; x++)
  //   {
  //     feld_t feld;
  //     insel.grafik_bebauung(feld, x, y, 0);
  //     if (feld.index != -1)
  //     {
  //       Bsh_bild& bsh = bsh_leser.gib_bsh_bild(feld.index);
  //       bs.zeichne_bsh_bild_oz(bsh, (x - y + height) * XRASTER, (x + y) * YRASTER + 2 * YRASTER - feld.grundhoehe * ELEVATION);
  //     }
  //   }
  // }

  // bs.exportiere_bmp(argv[2]);
}
