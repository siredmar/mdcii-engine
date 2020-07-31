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

#include <cstring>
#include <ios>
#include <iostream>
#include <memory>
#include <variant>

#include "gam/missions.hpp"

Mission4::Mission4(uint8_t* data, uint32_t length, const std::string& name)
    : name(name)
{
    Mission4Data m;
    // Multiple missions in one chunk laying adjacent in memory.
    int numMissions = length / sizeof(Mission4Data);
    for (int i = 0; i < numMissions; i++)
    {
        int missionLength = length / numMissions;
        memset((char*)&m, 0, missionLength);
        memcpy((char*)&m, data + (i * missionLength), missionLength);
        missions.push_back(m);
    }
}

Mission2::Mission2(uint8_t* data, uint32_t length, const std::string& name)
    : name(name)
{
    Mission2Data m;
    // Multiple missions in one chunk laying adjacent in memory.
    int numMissions = length / sizeof(Mission2Data);
    for (int i = 0; i < numMissions; i++)
    {
        int missionLength = length / numMissions;
        memset((char*)&m, 0, missionLength);
        memcpy((char*)&m, data + (i * missionLength), missionLength);
        missions.push_back(m);
    }
}