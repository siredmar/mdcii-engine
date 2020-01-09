
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

#ifndef BSH_SCHREIBER_HPP
#define BSH_SCHREIBER_HPP

#include <cstdint>
#include <vector>
#include <fstream>

class Bsh_schreiber
{
  int transp_farbe;
  int extra_spalten;
  bool ist_zei;

  struct Bild_und_meta
  {
    uint32_t breite, hoehe, typ, laenge, crc, versatz;
    int duplikat_von;
    std::vector<uint8_t> daten;
  };
  std::vector<Bild_und_meta> bilder;

public:
  Bsh_schreiber(int transp_farbe = 0, int extra_spalten = 0, bool ist_zei = false);
  ~Bsh_schreiber();
  void schreib_bsh(uint8_t* bild, int breite, int hoehe, std::vector<uint8_t>& ziel);
  void lies_pgm(const char* pfadname, uint8_t*& bild, int& breite, int& hoehe);
  void pgm_anhaengen(const char* pfadname);
  void datei_schreiben(const char* pfadname);
};

#endif
