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

#include "gam/scene.hpp"

SceneRanking::SceneRanking(uint8_t* data, uint32_t length, const std::string& name)
    : name(name)
{
    memcpy((char*)&sceneRanking, data, length);
}

SceneSave::SceneSave(uint8_t* data, uint32_t length, const std::string& name)
    : name(name)
{
    memcpy((char*)&sceneSave, data, length);
}

SceneCampaign::SceneCampaign(uint8_t* data, uint32_t length, const std::string& name)
    : name(name)
{
    memcpy((char*)&sceneCampaign, data, length);
}

SceneGameID::SceneGameID(uint8_t* data, uint32_t length, const std::string& name)
    : name(name)
{
    memcpy((char*)&sceneGameID, data, length);
}

SceneMissionNumber::SceneMissionNumber(uint8_t* data, uint32_t length, const std::string& name)
    : name(name)
{
    memcpy((char*)&sceneMissionNumber, data, length);
}