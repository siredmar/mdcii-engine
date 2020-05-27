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

#ifndef _PRODUCTIONLIST_HPP
#define _PRODUCTIONLIST_HPP

#include <inttypes.h>
#include <string>

enum class ProductionState : uint8_t
{
  Inactive = 0,
  Active = 1
};

enum class ConnectionToWarehouse : uint8_t
{
  NoConnection = 0,
  Connected = 1
};

enum class WarehouseCarrierFetch : uint8_t
{
  NotAllowed = 0,
  Allowed = 1
};

struct ProductionListData // PRODLIST2
{
  uint8_t inselnr;                      // Island number
  uint8_t posx;                         // position on the island
  uint8_t posy;                         // position on the island
  uint8_t speed;                        // Welcher Speedzähler (MAXWACHSSPEEDKIND)
  uint32_t speedcnt : 8;                // Wenn (Zeitzähler == Speedzähler) timer++ (MAXROHWACHSCNT)
  uint32_t lager : 24;                  // Lagerbestand an Fertigprodukten
  uint16_t timer;                       // Zeitzähler in Sekunden
  uint16_t worklager;                   // Bestand an Betriebsstoffen (vorwiegend Holz)
  uint32_t rohlager : 24;               // Lagerbestand an Rohstoffen
  uint32_t rohhebe : 8;                 // Was wird derzeit produziert??
  uint16_t productionCount;             // Wie oft wurde produziert ??
  uint16_t timecnt;                     // Zähler für Produktionsstatistik!!
  ProductionState aktivflg : 1;         // Produktionsstätte derzeit aktiv??
  ConnectionToWarehouse marktflg : 1;   // Eine Verbindung mit dem Hauptquartier besteht!!
  uint8_t animcnt : 4;                  // Aktuelle Animationsstufe (ACHTUNG: TISOFELD!!)
  WarehouseCarrierFetch nomarktflg : 1; // Marktfahrer dürfen hier keine Waren abholen
  uint8_t empty1 : 1;                   // Reserve ist immer gut
  uint8_t norohstcnt : 4;               // Seit längerer Zeit kein Rohstoff erreichbar
  uint8_t empty2;                       // Reserve ist immer gut
  uint8_t empty3;                       // Reserve ist immer gut
};

class ProductionList
{
public:
  explicit ProductionList(uint8_t* data, uint32_t length, const std::string& name);
  ProductionListData productionList;

private:
  std::string name;
};

#endif // _PRODUCTIONLIST_HPP