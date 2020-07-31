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

#ifndef _SHIPYARD_HPP
#define _SHIPYARD_HPP

#include <inttypes.h>
#include <string>
#include <vector>

struct ShipyardData // WERFT
{
    uint32_t inselnr : 8; //
    uint32_t posx : 8; //  Wo befindet sich Stätte
    uint32_t posy : 8; //
    uint32_t speedcnt : 3; //  Wenn (Zeitzähler == Speedzähler) timer++ (MAXROHWACHSCNT)
    uint32_t aktivflg : 1; //  Wird gerade an einem Schiff gebaut??
    uint32_t timer : 16; //  Zeitzähler in Sekunden
    uint32_t bauship : 8; //  Welcher Schiffstyp wird produziert
    uint32_t lager : 16; //  Wieviel Schiffsbauteile sind schon vorhanden
    uint32_t rohlager : 16; //  Lagerbestand an Rohstoffen
    uint32_t repshipnr : 16; //  Nummer des Schiffes das repariert wird
    uint32_t worklager : 16; //  Reserve ist immer gut
    uint32_t leer3 : 8; //  Reserve ist immer gut
    uint32_t leer4 : 8; //  Reserve ist immer gut
    uint32_t leer5; //  Reserve ist immer gut
};

class Shipyard
{
public:
    Shipyard()
    {
    }
    explicit Shipyard(uint8_t* data, uint32_t length, const std::string& name);
    std::vector<ShipyardData> shipyard;

private:
    std::string name;
};

#endif // _SHIPYARD_HPP