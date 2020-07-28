
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

#include <boost/format.hpp>
#include <boost/program_options.hpp>
#include <iostream>

#include "mdcii/bsh/bshreader.hpp"
#include "mdcii/files/files.hpp"
#include "mdcii/framebuffer/framebuffer_pal8.hpp"
#include "mdcii/framebuffer/framebuffer_rgb24.hpp"
#include "mdcii/framebuffer/palette.hpp"

namespace po = boost::program_options;

int main(int argc, char** argv)
{

  std::string input_name;
  std::string file_format;
  std::string prefix;
  int color;
  int bpp;

  // clang-format off
  po::options_description desc("Zulässige Optionen");
  desc.add_options()
    ("input,i", po::value<std::string>(&input_name), "Eingabedatei (*.bsh)")
    ("format,f", po::value<std::string>(&file_format)->default_value("pnm"), "Format (bmp oder pnm)")
    ("bpp,b", po::value<int>(&bpp)->default_value(24), "Bits pro Pixel (8 oder 24)")
    ("prefix,p", po::value<std::string>(&prefix)->default_value("g_"), "Präfix (inklusive Pfad) für die Zieldateinamen")
    ("color,c", po::value<int>(&color)->default_value(0), "Hintergrundfarbe für transparente Bereiche")
    ("path,l", po::value<std::string>()->default_value("."), "Pfad zur ANNO1602-Installation")
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

  if (vm.count("input") != 1)
  {
    std::cout << "Keine Eingabedatei angegeben" << std::endl;
    exit(EXIT_FAILURE);
  }

  if (bpp != 8 && bpp != 24)
  {
    std::cout << "Ungültige Angabe für die Anzahl an Bits pro Pixel" << std::endl;
    exit(EXIT_FAILURE);
  }

  if (file_format != "bmp" && file_format != "pnm")
  {
    std::cout << "Gültige Werte für --format sind bmp und pnm" << std::endl;
    exit(EXIT_FAILURE);
  }
  if (vm.count("path") != 1)
  {
    std::cout << "Keine Anno1602 Installation angegeben" << std::endl;
    exit(EXIT_FAILURE);
  }

  auto files = Files::CreateInstance(vm["path"].as<std::string>());
  Palette::CreateInstance(files->FindPathForFile("stadtfld.col"));

  BshReader bsh(input_name);
  if (bpp == 24)
  {
    for (uint32_t i = 0; i < bsh.Count(); i++)
    {
      BshImage& image = bsh.GetBshImage(i);
      FramebufferRgb24 fb(image.width, image.height, color);
      fb.Clear();
      fb.DrawBshImage(image, 0, 0);

      if (file_format == "pnm")
        fb.ExportPNM((prefix + boost::str(boost::format("%04d.ppm") % i)).c_str());
      else if (file_format == "bmp")
        fb.ExportBMP((prefix + boost::str(boost::format("%04d.bmp") % i)).c_str());
    }
  }
  else if (bpp == 8)
  {
    for (uint32_t i = 0; i < bsh.Count(); i++)
    {
      BshImage& image = bsh.GetBshImage(i);
      FramebufferPal8 fb(image.width, image.height, color);
      fb.Clear();
      fb.DrawBshImage(image, 0, 0);

      if (file_format == "pnm")
        fb.ExportPNM((prefix + boost::str(boost::format("%04d.pgm") % i)).c_str());
      else if (file_format == "bmp")
        fb.ExportBMP((prefix + boost::str(boost::format("%04d.bmp") % i)).c_str());
    }
  }
}
