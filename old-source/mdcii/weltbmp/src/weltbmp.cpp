
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
#include <stdlib.h>
#include <string.h>

#include <fstream>
#include <iostream>

#include "mdcii/bsh/bshreader.hpp"
#include "mdcii/cod/cod_parser.hpp"
#include "mdcii/files/files.hpp"
#include "mdcii/framebuffer/framebuffer_pal8.hpp"
#include "mdcii/framebuffer/palette.hpp"
#include "mdcii/insel.hpp"
#include "mdcii/version/version.hpp"
#include "mdcii/welt.hpp"

#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH

#define XRASTER 16
#define YRASTER 8
#define ELEVATION 10

/*
  Must be run from within the anno1602 folder
  usage: ./mdcii-weltbmp <savegame.sav> <output.bmp>
*/
int main(int argc, char** argv)
{
    if (argc < 3)
    {
        std::cout << "usage: ./mdcii-weltbmp <savegame.sav> <output.bmp>" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::ifstream f;
    f.open(argv[1], std::ios_base::in | std::ios_base::binary);

    auto files = Files::CreateInstance(".");
    Version::DetectGameVersion();
    Buildings::CreateInstance(std::make_shared<CodParser>(files->FindPathForFile("haeuser.cod"), true, false));
    Palette::CreateInstance(files->FindPathForFile("stadtfld.col"));
    Welt welt = Welt(f);

    f.close();
    BshReader bsh_leser(files->FindPathForFile("mgfx/stadtfld.bsh"));

    FramebufferPal8 fb((Welt::KARTENBREITE + Welt::KARTENHOEHE) * XRASTER, (Welt::KARTENBREITE + Welt::KARTENHOEHE) * YRASTER, 0);

    for (int y = 0; y < Welt::KARTENHOEHE; y++)
    {
        for (int x = 0; x < Welt::KARTENBREITE; x++)
        {
            Insel* insel = welt.insel_an_pos(x, y);
            feld_t feld;
            if (insel != NULL)
                insel->grafik_bebauung(feld, x - insel->xpos, y - insel->ypos, 0);
            else
            {
                feld.index = Buildings::Instance()->GetHouse(1201).value()->Gfx + (y + x * 3) % 12;
                feld.grundhoehe = 0;
            }
            /*feld_t feld2;
      insel->grafik_boden(&feld2, x, y, 0);*/
            if (feld.index != -1)
            {
                BshImage& bsh = bsh_leser.GetBshImage(feld.index);
                uint16_t x_auf_karte = x /*- insel->width / 2*/;
                uint16_t y_auf_karte = y /*- insel->height / 2*/;
                fb.zeichne_bsh_bild_oz(
                    bsh, (x_auf_karte - y_auf_karte + Welt::KARTENHOEHE) * XRASTER, (x_auf_karte + y_auf_karte) * YRASTER + 2 * YRASTER - feld.grundhoehe * ELEVATION);
            }
            /*else
        std::cout << insel->schicht2[y * insel->width + x].bebauung << " ";*/
        }
    }

    fb.ExportBMP(argv[2]);
}
