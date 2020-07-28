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

#include "mdcii/bsh/zeireader.hpp"
#include "mdcii/files/files.hpp"
#include "mdcii/framebuffer/framebuffer_pal8.hpp"
#include "mdcii/framebuffer/framebuffer_rgb24.hpp"
#include "mdcii/framebuffer/palette.hpp"

namespace po = boost::program_options;

using namespace std;

int main(int argc, char** argv)
{
  string input_name;
  string file_format;
  string output;
  string text;
  string path;
  int color;
  int bpp;

  // clang-format off
  po::options_description desc("Zulässige Optionen");
  desc.add_options()
    ("input,i", po::value<string>(&input_name), "Eingabedatei (*.zei)")
    ("format,f", po::value<string>(&file_format)->default_value("pnm"), "Format (bmp oder pnm)")
    ("bpp,b", po::value<int>(&bpp)->default_value(24), "Bits pro Pixel (8 oder 24)")
    ("output,o", po::value<string>(&output), "Name der Ausgabedatei")
    ("color,c", po::value<int>(&color)->default_value(128), "Hintergrundfarbe für transparente Bereiche")
    ("text,t", po::value<string>(&text), "Text, der Ausgegeben werden soll")
    ("path,p", po::value<string>(&path), "ANNO1602")
    ("help,h", "Gibt diesen Hilfetext aus")
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

  if (vm.count("output") != 1)
  {
    cout << "Keine Ausgabedatei angegeben" << endl;
    exit(EXIT_FAILURE);
  }

  if (bpp != 8 && bpp != 24)
  {
    cout << "Ungültige Angabe für die Anzahl an Bits pro Pixel" << endl;
    exit(EXIT_FAILURE);
  }

  if (file_format != "bmp" && file_format != "pnm")
  {
    cout << "Gültige Werte für --format sind bmp und pnm" << endl;
    exit(EXIT_FAILURE);
  }
  auto files = Files::CreateInstance(path);
  Palette::CreateInstance(files->FindPathForFile("stadtfld.col"));

  ZeiReader zei(input_name);
  unsigned int zeimaxbreite = 0;
  unsigned int zeimaxhoehe = 0;
  unsigned int width = 0;
  unsigned int height = 0;
  unsigned int colums = 10;

  for (unsigned int i = 0; i < zei.Count(); i++)
  {
    ZeiCharacter& z = zei.GetBshImage(i);
    if (z.width > zeimaxbreite)
    {
      zeimaxbreite = z.width;
    }
    if (z.height > zeimaxhoehe)
    {
      zeimaxhoehe = z.height;
    }
  }
  width = 10 * zeimaxbreite;
  height = (zei.Count() / colums + 1) * zeimaxhoehe;

  if (bpp == 24)
  {
    FramebufferRgb24 fb(width, height, color);
    fb.SetFontColor(255, 0);
    fb.Clear();
    int x, y = 0;
    for (unsigned int i = 0; i < zei.Count(); i++)
    {
      fb.DrawZeiCharacter(zei.GetBshImage(i), x, y);
      if (i % colums == 0)
      {
        y += zeimaxhoehe;
        x = 0;
      }
      else
      {
        x += zeimaxbreite;
      }
    }

    if (file_format == "pnm")
      fb.ExportPNM((output + ".ppm").c_str());
    else if (file_format == "bmp")
      fb.ExportBMP((output + ".bmp").c_str());
  }
  else if (bpp == 8)
  {
    FramebufferPal8 fb(width, height, color);
    fb.SetFontColor(255, 0);
    fb.Clear();
    int x, y = 0;
    for (unsigned int i = 0; i < zei.Count(); i++)
    {
      if (i % colums == 0)
      {
        y += zeimaxhoehe;
        x = 0;
      }
      else
      {
        x += zeimaxbreite;
      }
      fb.DrawZeiCharacter(zei.GetBshImage(i), x, y);
    }

    if (file_format == "pnm")
      fb.ExportPNM((output + ".pgm").c_str());
    else if (file_format == "bmp")
      fb.ExportBMP((output + ".bmp").c_str());
  }
}
