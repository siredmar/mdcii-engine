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

#ifndef _MARKETPLACE_HPP
#define _MARKETPLACE_HPP

#include <inttypes.h>
#include <string>
#include <vector>

struct MarketPlaceGoods
{
    uint32_t goodConsumption; //  Letzter Verbrauch
    uint32_t goodCoverRatio; //  Deckungssatz der Bedürfnisse
    uint32_t empty0; //  empty
    uint16_t houseId; //  Hausnummer für Warentyp !!
    uint8_t goodSupplyLevel; //  Versorgungsgrad mit dieser Ware
    uint8_t empty1; //  empty
};

struct MarketPlaceData // MARKT
{
    uint8_t islandNumber : 8;
    uint8_t cityNumber : 4; // Auf welcher Insel und Stadt??
    MarketPlaceGoods goods[16]; // ACHTUNG: Falls MARKT_WAREMAX > 16
};

class MarketPlace
{
public:
    MarketPlace()
    {
    }
    explicit MarketPlace(uint8_t* data, uint32_t length, const std::string& name);
    std::vector<MarketPlaceData> marketPlace;

private:
    std::string name;
};

#endif // _MARKETPLACE_HPP