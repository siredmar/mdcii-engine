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

#ifndef _SCENARIOS_H_
#define _SCENARIOS_H_

#include <experimental/optional>
#include <string>
#include <tuple>
#include <vector>

#include "gamelist.pb.h"

#include "files/files.hpp"

class Scenarios
{
public:
    explicit Scenarios(const std::string& basepath, const std::string& fileEnding);
    unsigned int size() const;
    GamesPb::Games Get() const;

private:
    int CampagneIndex(const std::string& name);
    void SortCampaigns();
    void SortOriginalMissions();
    void SortCampaignMissions(GamesPb::Campaign* campaign);
    GamesPb::Games gamelist;
};

#endif // _SAVEGAMES_H_