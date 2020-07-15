
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

#include "mdcii/bsh/bshreader.hpp"
#include "mdcii/cod/cod_parser.hpp"
#include "mdcii/files/files.hpp"
#include "mdcii/framebuffer/framebuffer_pal8.hpp"
#include "mdcii/framebuffer/palette.hpp"
#include "mdcii/insel.hpp"
#include "mdcii/version/version.hpp"


#define XRASTER 32
#define YRASTER 16
#define ELEVATION 20

/*
  Must be run from within the anno1602 folder
  usage: ./mdcii-inselbmp <island.scp> <output.bmp>
*/
int main(int argc, char** argv)
{
  if (argc < 4)
  {
    std::cout << "usage: ./mdcii-inselbmp <island.scp> <output.bmp> <anno path>" << std::endl;
    exit(EXIT_FAILURE);
  }

  std::ifstream f;
  f.open(argv[1], std::ios_base::in | std::ios_base::binary);

  Block inselX = Block(f);
  Block inselhaus = Block(f);

  f.close();

  auto files = Files::CreateInstance(std::string(argv[3]));
  Buildings::CreateInstance(std::make_shared<CodParser>(files->Instance()->FindPathForFile("haeuser.cod"), true, false));
  Palette::CreateInstance(files->Instance()->FindPathForFile("stadtfld.col"));
  Version::DetectGameVersion();

  Insel insel = Insel(&inselX, &inselhaus, Buildings::Instance());
  uint8_t width = insel.width;
  uint8_t height = insel.height;

  BshReader bsh_leser(files->Instance()->FindPathForFile("/gfx/stadtfld.bsh"));

  FramebufferPal8 fb((width + height) * XRASTER, (width + height) * YRASTER, 0);

  int x, y;
  for (y = 0; y < height; y++)
  {
    for (x = 0; x < width; x++)
    {
      if (x == 10 && y == 10)
      {
        std::cout << "hit" << std::endl;
      }
      feld_t feld;
      insel.grafik_bebauung(feld, x, y, 0);
      if (feld.index != -1)
      {
        BshImage& bsh = bsh_leser.GetBshImage(feld.index);
        fb.zeichne_bsh_bild_oz(bsh, (x - y + height) * XRASTER, (x + y) * YRASTER + 2 * YRASTER - feld.grundhoehe * ELEVATION);
      }
    }
  }

  fb.ExportBMP(argv[2]);
}
