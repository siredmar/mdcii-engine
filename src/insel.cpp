
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

#include "strukturen.hpp"
#include "insel.hpp"
#include "files.hpp"
#include "haeuser.hpp"
#include <string.h>
#include <fstream>
#include <boost/format.hpp>
#include <string>
#include <iostream>

void Insel::insel_rastern(inselfeld_t* a, uint32_t laenge, inselfeld_t* b, uint8_t breite, uint8_t hoehe)
{
  for (int y = 0; y < hoehe; y++)
  {
    for (int x = 0; x < breite; x++)
    {
      b[y * breite + x].bebauung = 0xffff;
    }
  }

  for (int i = 0; i < laenge; i++)
  {
    inselfeld_t& feld = a[i];

    if (feld.x_pos >= breite || feld.y_pos >= hoehe)
      continue;

    auto info = haeuser->get_haus(feld.bebauung);
    if (info)
    {
      int x, y, u, v;
      if (feld.rot % 2 == 0)
      {
        u = info.value()->Size.w;
        v = info.value()->Size.h;
      }
      else
      {
        u = info.value()->Size.h;
        v = info.value()->Size.w;
      }

      for (int y = 0; y < v && feld.y_pos + y < hoehe; y++)
      {
        for (int x = 0; x < u && feld.x_pos + x < breite; x++)
        {
          b[(feld.y_pos + y) * breite + feld.x_pos + x] = feld;
          b[(feld.y_pos + y) * breite + feld.x_pos + x].x_pos = x;
          b[(feld.y_pos + y) * breite + feld.x_pos + x].y_pos = y;
        }
      }
    }
    else
    {
      b[feld.y_pos * breite + feld.x_pos] = feld;
      b[feld.y_pos * breite + feld.x_pos].x_pos = 0;
      b[feld.y_pos * breite + feld.x_pos].y_pos = 0;
    }
  }
}

/*30x30 "lit%02d.scp"
40x40 "mit%02d.scp"
50x52 "med%02d.scp"
70x60 "big%02d.scp"
100x90 "lar%02d.scp"
*/

std::string Insel::basisname(uint8_t breite, uint8_t num, uint8_t sued)
{
  // 30x30
  if (breite < 35)
    return boost::str(boost::format("%slit%02d.scp") % (sued ? "sued/" : "nord/") % (int)num);
  // 40x40
  if (breite < 45)
    return boost::str(boost::format("%smit%02d.scp") % (sued ? "sued/" : "nord/") % (int)num);
  // 50x52
  if (breite < 60)
    return boost::str(boost::format("%smed%02d.scp") % (sued ? "sued/" : "nord/") % (int)num);
  // 70x60
  if (breite < 85)
    return boost::str(boost::format("%sbig%02d.scp") % (sued ? "sued/" : "nord/") % (int)num);
  // 100x90
  return boost::str(boost::format("%slar%02d.scp") % (sued ? "sued/" : "nord/") % (int)num);
}

Insel::Insel(Block* inselX, Block* inselhaus, std::shared_ptr<Haeuser> haeuser)
  : haeuser(haeuser)
{
  // Kennung prüfen
  if (strcmp(inselX->kennung, Insel5::kennung) == 0)
  {
    this->inselX = inselX;
    this->breite = ((Insel5*)inselX->daten)->breite;
    this->hoehe = ((Insel5*)inselX->daten)->hoehe;
    this->xpos = ((Insel5*)inselX->daten)->x_pos;
    this->ypos = ((Insel5*)inselX->daten)->y_pos;
    this->schicht1 = new inselfeld_t[this->breite * this->hoehe];
    this->schicht2 = new inselfeld_t[this->breite * this->hoehe];
    if (((Insel5*)inselX->daten)->diff == 0)
    {
      std::ifstream f;
      auto files = Files::instance();
      std::string karte = this->basisname(this->breite, ((Insel5*)inselX->daten)->basis, ((Insel5*)inselX->daten)->sued).c_str();
      karte = files->instance()->find_path_for_file(karte);
      if (files->instance()->check_file(karte) == false)
      {
        std::cout << "[ERR] Island not found: " << karte << std::endl;
        exit(EXIT_FAILURE);
      }

      f.open(karte, std::ios_base::in | std::ios_base::binary);
      Block inselX_basis = Block(f);
      Block inselhaus_basis = Block(f);
      this->insel_rastern((inselfeld_t*)inselhaus_basis.daten, inselhaus_basis.laenge / 8, schicht1, this->breite, this->hoehe);
      f.close();
    }

    this->insel_rastern((inselfeld_t*)inselhaus->daten, inselhaus->laenge / 8, schicht2, this->breite, this->hoehe);

    uint8_t x, y;
    for (y = 0; y < this->hoehe; y++)
    {
      for (x = 0; x < this->breite; x++)
      {
        if (schicht2[y * this->breite + x].bebauung == 0xffff)
          schicht2[y * this->breite + x] = schicht1[y * this->breite + x];
      }
    }
  }
  else if (strcmp(inselX->kennung, Insel3::kennung) == 0)
  {
    this->inselX = inselX;
    this->breite = ((Insel3*)inselX->daten)->breite;
    this->hoehe = ((Insel3*)inselX->daten)->hoehe;
    this->xpos = ((Insel3*)inselX->daten)->x_pos;
    this->ypos = ((Insel3*)inselX->daten)->y_pos;
    this->schicht1 = new inselfeld_t[this->breite * this->hoehe];
    this->schicht2 = new inselfeld_t[this->breite * this->hoehe];
    this->insel_rastern((inselfeld_t*)inselhaus->daten, inselhaus->laenge / 8, schicht1, this->breite, this->hoehe);
    memcpy(schicht2, schicht1, sizeof(inselfeld_t) * this->breite * this->hoehe);
  }
}

void Insel::grafik_boden(feld_t& ziel, uint8_t x, uint8_t y, uint8_t r)
{
  auto info = haeuser->get_haus(schicht2[y * breite + x].bebauung);
  int grafik;
  if (info)
  {
    if (schicht2[y * breite + x].bebauung != 0xffff)
    {
      grafik = info.value()->Gfx;
      if ((info.value()->Posoffs == 0 ? 0 : 1) == 0 && grafik != -1)
      {
        ziel.index = grafik;
        ziel.grundhoehe = info.value()->Posoffs == 0 ? 0 : 1;
        return;
      }
    }
    if (schicht1[y * breite + x].bebauung != 0xffff)
    {
      grafik = info.value()->Gfx;
      if (info.value()->Highflg == 0 && grafik != -1)
      {
        ziel.index = grafik;
        ziel.grundhoehe = info.value()->Posoffs == 0 ? 0 : 1;
        return;
      }
    }
  }
  ziel.index = 0;
  ziel.grundhoehe = 1;
}

void Insel::inselfeld_bebauung(inselfeld_t& ziel, uint8_t x, uint8_t y)
{
  uint8_t xp = schicht2[y * breite + x].x_pos;
  uint8_t yp = schicht2[y * breite + x].y_pos;
  if ((yp > y) || (xp > x))
  {
    // TODO "ziel" auf einen sinnvollen Wert setzen
    return;
  }
  ziel = schicht2[(y - yp) * breite + x - xp];
  ziel.x_pos = xp;
  ziel.y_pos = yp;
}

void Insel::grafik_bebauung_inselfeld(feld_t& ziel, inselfeld_t& feld, uint8_t r, std::shared_ptr<Haeuser> haeuser)
{
  if (feld.bebauung == 0xffff)
  {
    ziel.index = -1;
    ziel.grundhoehe = 0;
    return;
  }
  auto info = haeuser->get_haus(feld.bebauung);

  if (!info || info.value()->Gfx == -1)
  {
    ziel.index = -1;
    ziel.grundhoehe = 0;
    return;
  }
  int grafik = info.value()->Gfx;
  int index = grafik;
  int richtungen = 1;
  if (info.value()->Rotate > 0)
  {
    richtungen = 4;
  }
  int ani_schritte = 1;
  if (info.value()->AnimAnz > 0)
  {
    ani_schritte = info.value()->AnimAnz;
  }
  index += info.value()->Rotate * ((r + feld.rot) % richtungen);
  switch (feld.rot)
  {
    case 0: index += feld.y_pos * info.value()->Size.w + feld.x_pos; break;
    case 1: index += (info.value()->Size.h - feld.x_pos - 1) * info.value()->Size.w + feld.y_pos; break;
    case 2: index += (info.value()->Size.h - feld.y_pos - 1) * info.value()->Size.w + (info.value()->Size.w - feld.x_pos - 1); break;
    case 3: index += feld.x_pos * info.value()->Size.w + (info.value()->Size.w - feld.y_pos - 1); break;
  }
  index += info.value()->Size.h * info.value()->Size.w * richtungen * (feld.ani % ani_schritte);
  ziel.index = index;
  int grundhoehe = 0;
  if (info.value()->Posoffs == 20)
  {
    grundhoehe = 1;
  }
  ziel.grundhoehe = grundhoehe;
}

void Insel::grafik_bebauung(feld_t& ziel, uint8_t x, uint8_t y, uint8_t r)
{
  inselfeld_t feld;
  inselfeld_bebauung(feld, x, y);
  grafik_bebauung_inselfeld(ziel, feld, r, haeuser);
}

void Insel::bewege_wasser() // FIXME
{
  for (int y = 0; y < hoehe; y++)
  {
    for (int x = 0; x < breite; x++)
    {
      inselfeld_t& feld = schicht2[y * breite + x];
      if (feld.bebauung >= 1201 && feld.bebauung <= 1218 || feld.bebauung >= 901 && feld.bebauung <= 905 || feld.bebauung >= 1251 && feld.bebauung <= 1259
          || feld.bebauung == 1071 || feld.bebauung == 2311)
      {
        auto info = haeuser->get_haus(feld.bebauung);
        if (info)
        {
          if (info.value()->AnimAnz > 0)
          {
            feld.ani = (feld.ani + 1) % (info.value()->AnimAnz);
          }
        }
      }
    }
  }
}

void Insel::animiere_gebaeude(uint8_t x, uint8_t y)
{
  inselfeld_t& feld = schicht2[y * breite + x];
  auto info = haeuser->get_haus(feld.bebauung);
  if (info)
  {
    if (info.value()->AnimTime != -1)
    {
      int animAnz = 1;
      if (info.value()->AnimAnz > 0)
      {
        feld.ani = (feld.ani + 1) % (info.value()->AnimAnz);
      }
    }
  }
}
