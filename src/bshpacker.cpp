
// This file is part of the MDCII Game Engine.
// Copyright (C) 2017  Benedikt Freisen
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
#include <boost/format.hpp>
#include <boost/program_options.hpp>

#include "bsh_schreiber.hpp"

namespace po = boost::program_options;

using namespace std;

int main(int argc, char** argv)
{
  string output_name;
  string file_format;
  string prefix;
  string signature;
  int number;
  int color;
  int bpp;
  int extra;

  // clang-format off
  po::options_description desc("Zulässige Optionen");
  desc.add_options()
    ("output,o", po::value<string>(&output_name), "Zieldatei (*.bsh, *.zei)")
    ("signature,s", po::value<string>(&signature)->default_value("BSH"), "Signatur (BSH oder ZEI)")
    ("format,f", po::value<string>(&file_format)->default_value("pnm"), "Format (bmp oder pnm)")
    ("bpp,b", po::value<int>(&bpp)->default_value(8), "Bits pro Pixel (8 oder 24)")
    ("prefix,p", po::value<string>(&prefix)->default_value("g_"), "Präfix (inklusive Pfad) für die Namen der Ausgangsdateien")
    ("number,n", po::value<int>(&number)->default_value(1), "Anzahl zu verarbeitenden Bilder (mit Nummern 0 bis n-1)")
    ("color,c", po::value<int>(&color)->default_value(0), "Hintergrundfarbe für transparente Bereiche")
    ("extra,e", po::value<int>(&extra)->default_value(0), "Extraspalten in den Ausgangsdateien")
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

  if (vm.count("output") != 1)
  {
    cout << "Keine Zieldatei angegeben" << endl;
    exit(EXIT_FAILURE);
  }

  if (bpp != 8 && bpp != 24)
  {
    cout << "Ungültige Angabe für die Anzahl an Bits pro Pixel" << endl;
    exit(EXIT_FAILURE);
  }

  if (number < 1)
  {
    cout << "Der Wert für --number muss größer als oder gleich 1 sein" << endl;
    exit(EXIT_FAILURE);
  }

  if (file_format != "bmp" && file_format != "pnm")
  {
    cout << "Gültige Werte für --format sind bmp und pnm" << endl;
    exit(EXIT_FAILURE);
  }

  if (signature != "BSH" && signature != "ZEI")
  {
    cout << "Gültige Werte für die --signature sind BSH und ZEI" << endl;
    exit(EXIT_FAILURE);
  }

  Bsh_schreiber bsh(color, extra, signature == "ZEI");
  if (bpp == 24)
  {
    for (uint32_t i = 0; i < number; i++)
    {
      // TODO
    }
  }
  else if (bpp == 8)
  {
    for (uint32_t i = 0; i < number; i++)
    {
      if (file_format == "pnm")
        bsh.pgm_anhaengen((prefix + boost::str(boost::format("%04d.pgm") % i)).c_str());
      // else if (file_format == "bmp")
      //   bsh.bmp_anhaengen((prefix + boost::str(boost::format("%04d.bmp") % i)).c_str());
    }
  }
  bsh.datei_schreiben(output_name.c_str());
}
