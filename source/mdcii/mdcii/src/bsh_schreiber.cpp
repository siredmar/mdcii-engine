
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

#include "bsh_schreiber.hpp"

#include <boost/crc.hpp>

using namespace std;

istream& operator>>(istream& is, const char* str)
{
  string temp;
  is >> temp;
  if (temp != str)
    throw new istream::failure("unexpected string constant in input");
  return is;
}

Bsh_schreiber::Bsh_schreiber(int transp_farbe, int extra_spalten, bool ist_zei)
{
  this->transp_farbe = transp_farbe;
  this->extra_spalten = extra_spalten;
  this->ist_zei = ist_zei;
}

Bsh_schreiber::~Bsh_schreiber()
{
}

enum zustand_t
{
  TRANSPARENT,
  PIXEL,
};

#define ist_transparent(a) ((a) == transp_farbe)

void Bsh_schreiber::schreib_bsh(uint8_t* bild, int breite, int hoehe, vector<uint8_t>& ziel)
{
  int tz = 0;
  int zz = 0;
  int pixel_index = 0;
  zustand_t zustand = TRANSPARENT;

  for (int y = 0; y < hoehe; y++)
  {
    for (int x = 0; x < breite; x++)
    {
      if (zustand == TRANSPARENT)
      {
        if (x == 0 && y != 0)
        {
          zz++;
          tz = 0;
        }
        if (ist_transparent(bild[y * breite + x]))
        {
          tz++;
        }
        else
        {
          for (int i = 0; i < zz; i++)
          {
            ziel.push_back(0xfe);
          }
          zz = 0;
          while (tz > 253)
          {
            ziel.push_back(253);
            ziel.push_back(0);
            tz -= 253;
          }
          ziel.push_back(tz);
          tz = 1;
          pixel_index = y * breite + x;
          zustand = PIXEL;
        }
      }
      else
      {
        if (x == 0 || ist_transparent(bild[y * breite + x]))
        {
          // tz Pixel ausgeben
          while (tz > 253)
          {
            ziel.push_back(253);
            for (int i = 0; i < 253; i++)
            {
              ziel.push_back(bild[pixel_index]);
              pixel_index++;
            }
            ziel.push_back(0);
            tz -= 253;
          }
          ziel.push_back(tz);
          for (int i = 0; i < tz; i++)
          {
            ziel.push_back(bild[pixel_index]);
            pixel_index++;
          }
          tz = 1;
          if (x == 0 && !ist_transparent(bild[y * breite + x]))
          {
            ziel.push_back(0xfe);
            ziel.push_back(0);
          }
          else
          {
            if (ist_transparent(bild[y * breite + x]))
            {
              zustand = TRANSPARENT;
              if (x == 0)
              {
                zz++;
              }
            }
          }
        }
        else
        {
          tz++;
        }
      }
    }
  }
  if (zustand == PIXEL)
  {
    // tz Pixel ausgeben
    while (tz > 253)
    {
      ziel.push_back(253);
      for (int i = 0; i < 253; i++)
      {
        ziel.push_back(bild[pixel_index]);
        pixel_index++;
      }
      ziel.push_back(0);
      tz -= 253;
    }
    ziel.push_back(tz);
    for (int i = 0; i < tz; i++)
    {
      ziel.push_back(bild[pixel_index]);
      pixel_index++;
    }
  }
  ziel.push_back(0xff);
}

void Bsh_schreiber::lies_pgm(const char* pfadname, uint8_t*& bild, int& breite, int& hoehe)
{
  ifstream pgm;
  pgm.open(pfadname, ios_base::in | ios_base::binary);
  pgm.unsetf(ios::skipws);
  pgm >> "P5";
  pgm.setf(ios::skipws);
  pgm >> breite >> hoehe >> "255";
  pgm.get();
  bild = new uint8_t[breite * hoehe];
  pgm.read(reinterpret_cast<char*>(bild), breite * hoehe);
  pgm.close();
}

void Bsh_schreiber::pgm_anhaengen(const char* pfadname)
{
  int breite, hoehe;
  uint8_t* bild;
  lies_pgm(pfadname, bild, breite, hoehe);
  bilder.emplace_back();
  Bild_und_meta& ziel = bilder.back();
  schreib_bsh(bild, breite, hoehe, ziel.daten);
  delete[] bild;
  while (ziel.daten.size() % 4 != 0)
    ziel.daten.push_back(0);
  ziel.typ = 1;
  ziel.laenge = ziel.daten.size() + 16;
  ziel.breite = (breite > extra_spalten) ? breite - extra_spalten : 1;
  ziel.hoehe = hoehe;
  boost::crc_32_type crccomp;
  crccomp.process_bytes(ziel.daten.data(), ziel.daten.size());
  ziel.crc = crccomp.checksum();
  ziel.duplikat_von = -1;
  ziel.versatz = 0;
}

void Bsh_schreiber::datei_schreiben(const char* pfadname)
{
  if (bilder.size() < 1)
    return;

  fstream bsh;
  bsh.open(pfadname, fstream::in | fstream::out | fstream::trunc | fstream::binary);
  char signatur_bsh[16] = {'B', 'S', 'H', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'};
  char signatur_zei[16] = {'Z', 'E', 'I', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'};
  if (ist_zei)
    bsh.write(signatur_zei, sizeof(signatur_zei));
  else
    bsh.write(signatur_bsh, sizeof(signatur_bsh));

  for (int i = 0; i < bilder.size(); i++)
  {
    for (int j = 0; j < i; j++)
    {
      if (bilder[i].crc == bilder[j].crc && bilder[i].breite == bilder[j].breite && bilder[i].hoehe == bilder[j].hoehe && bilder[i].typ == bilder[j].typ
          && bilder[i].laenge == bilder[j].laenge && bilder[i].daten == bilder[j].daten)
      {
        bilder[i].duplikat_von = j;
        break;
      }
    }
  }

  uint32_t versatz_absolut = bilder.size() * sizeof(uint32_t);
  for (int i = 0; i < bilder.size(); i++)
  {
    if (bilder[i].duplikat_von == -1)
    {
      bilder[i].versatz = versatz_absolut;
      versatz_absolut += bilder[i].laenge;
    }
    else
      bilder[i].versatz = bilder[bilder[i].duplikat_von].versatz;
  }

  uint32_t groesse = bilder.size() * sizeof(uint32_t);
  for (Bild_und_meta& bild : bilder)
  {
    if (bild.duplikat_von == -1)
      groesse += bild.laenge;
  }
  bsh.write(reinterpret_cast<char*>(&groesse), sizeof(groesse));

  for (Bild_und_meta& bild : bilder)
  {
    bsh.write(reinterpret_cast<char*>(&bild.versatz), sizeof(bild.versatz));
  }

  for (Bild_und_meta& bild : bilder)
  {
    if (bild.duplikat_von == -1)
    {
      bsh.write(reinterpret_cast<char*>(&bild.breite), sizeof(bild.breite));
      bsh.write(reinterpret_cast<char*>(&bild.hoehe), sizeof(bild.hoehe));
      bsh.write(reinterpret_cast<char*>(&bild.typ), sizeof(bild.typ));
      bsh.write(reinterpret_cast<char*>(&bild.laenge), sizeof(bild.laenge));
      bsh.write(reinterpret_cast<char*>(bild.daten.data()), bild.daten.size());
    }
  }
}
