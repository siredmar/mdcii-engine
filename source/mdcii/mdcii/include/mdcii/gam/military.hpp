// This file is part of the MDCII Game Engine.
// Copyright (C) 2020  Armin Schlegel
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

#ifndef _MILITARY_HPP
#define _MILITARY_HPP

#include <inttypes.h>
#include <string>
#include <vector>


struct MilitaryUnitData
{
  uint32_t figurnr : 12; // Welche Figur befindet sich in der Kaserne?? (FIGUR_MAX <= 256!!)
  uint32_t trainflg : 1; // Wird gerade eine Einheit ausgebildet
  uint32_t waffflg : 1;  // Waffe ist für diese Figur bereits vorhanden
  uint32_t leer1 : 18;   // Reserve ist immer gut
  uint32_t energy : 16;  // Aktueller Energypegel
  uint32_t expert : 16;  // Erreichter Ausbildungsstand
};

struct MilitaryData // MILITAR
{
  uint32_t inselnr : 8;         //
  uint32_t posx : 8;            // Standort des Militärischen Gebäudes
  uint32_t posy : 8;            //
  uint32_t speedcnt : 8;        // Wenn (Zeitzähler == Speedzähler) timer++ (MAXROHWACHSCNT)
  uint32_t timer : 16;          // Zeit bis zur nächsten Produktionsrunde
  uint16_t wafflager[4];        // Aktueller Waffenbestand in der Kaserne
  MilitaryUnitData objlist[12]; // Objekte in der Kaserne !!
};

class Military
{
public:
  Military()
  {
  }
  explicit Military(uint8_t* data, uint32_t length, const std::string& name);
  std::vector<MilitaryData> military;

private:
  std::string name;
};

#endif // _MILITARY_HPP