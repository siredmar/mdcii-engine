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

#ifndef _SETTLERS_HPP
#define _SETTLERS_HPP

#include <inttypes.h>
#include <string>
#include <vector>

enum class CitizenGroupSettlers : uint8_t
{
    Pioneer = 0,
    Settlers = 1,
    Citizens = 2,
    Merchants = 3,
    Aristocrats = 4
};

struct SettlersData // Siedler
{
    uint8_t inselnr; // Number of island
    uint8_t posx; // Position on island
    uint8_t posy; // Position on island
    uint8_t speedcnt; // If (Timecnt == Speedcnt) timer++ (MAXROHWACHSCNT)
    uint8_t stadtcnt; // Flag if Reproduction is recalculated
    CitizenGroupSettlers citizenGroup; // Which citizen group
    uint8_t WarehouseDistance; // Distance to warehouse (MAXMARKTDIST)
    uint16_t count; // citicens in this house * 64, e.g. 2 * 64 = 128 (count)
    uint8_t speed : 4; // Welcher Zeitzähler gilt ??
    uint8_t empty0 : 1; // Wird zur Bevölkerungsgruppenr. hinzugezählt
    uint8_t InfrastructureFlag : 1; // Flags that represents needed infrastructure bildings
    bool NearWarehouse : 1; // Near warehouse?
    bool NearChurch : 1; // Near church?
    bool NearTavern : 1; // Near tavern?
    bool NearBathHouse : 1; // Near bath house?
    bool NearTheater : 1; // Near theater?
    bool NearHeardquarter : 1; // Near headquarter?
    bool NearSchool : 1; // Near school
    bool NearUniversity : 1; // Near university?
    bool NearChapel : 1; // Near chapel?
    bool NearClinic : 1; // Near clinic?
    uint8_t empty1 : 2; // empty
    uint8_t empty2; // empty
    uint8_t empty3; // empty
    uint8_t empty4; // empty
};

class Settlers
{
public:
    Settlers()
    {
    }
    explicit Settlers(uint8_t* data, uint32_t length, const std::string& name);
    std::vector<SettlersData> settlers;

private:
    std::string name;
};

#endif // _SETTLERS_HPP