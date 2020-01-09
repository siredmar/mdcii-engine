
// This file is part of the MDCII Game Engine.
// Copyright (C) 2015-2018  Benedikt Freisen
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

#include <iostream>
#include <boost/program_options.hpp>

#include "zei_leser.hpp"
#include "bildspeicher_rgb24.hpp"
#include "bildspeicher_pal8.hpp"

namespace po = boost::program_options;

using namespace std;

int main(int argc, char** argv)
{
  string input_name;
  string file_format;
  string output;
  string text;
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

  Zei_leser zei(input_name);
  vector<Zei_zeichen*> zeichen;
  int breite = 0;
  int hoehe = 0;
  for (char c : text)
  {
    Zei_zeichen& z = zei.gib_bsh_bild(c - ' ');
    zeichen.push_back(&z);
    breite += z.breite;
    if (z.hoehe > hoehe)
      hoehe = z.hoehe;
  }
  int position = 0;
  if (bpp == 24)
  {
    Bildspeicher_rgb24 bs(breite, hoehe, color);
    bs.setze_schriftfarbe(255, 0);
    bs.bild_loeschen();
    bs.zeichne_string(zei, text, 0, 0);

    if (file_format == "pnm")
      bs.exportiere_pnm((output + ".ppm").c_str());
    else if (file_format == "bmp")
      bs.exportiere_bmp((output + ".bmp").c_str());
  }
  else if (bpp == 8)
  {
    Bildspeicher_pal8 bs(breite, hoehe, color);
    bs.setze_schriftfarbe(255, 0);
    bs.bild_loeschen();
    bs.zeichne_string(zei, text, 0, 0);

    if (file_format == "pnm")
      bs.exportiere_pnm((output + ".pgm").c_str());
    else if (file_format == "bmp")
      bs.exportiere_bmp((output + ".bmp").c_str());
  }
}
