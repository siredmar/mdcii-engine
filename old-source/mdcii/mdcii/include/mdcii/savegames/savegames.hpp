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

#ifndef _SAVEGAMES_H_
#define _SAVEGAMES_H_

#include <experimental/optional>
#include <string>
#include <tuple>
#include <vector>

#include "gamelist.pb.h"

#include "files/files.hpp"

class Savegames
{
public:
    explicit Savegames(const std::string& basepath, const std::string& fileEnding);
    unsigned int size() const;
    std::experimental::optional<std::string> GetPath(unsigned int index) const;
    std::experimental::optional<std::string> GetName(unsigned int index) const;
    std::experimental::optional<int> GetRanking(unsigned int index) const;
    std::vector<std::tuple<std::string, std::string, int>> GetSavegames() const;
    // GamesPb::Games Get() const;

private:
    // vector element contains: path, name, ranking
    // GamesPb::Games gamelist;
    std::vector<std::tuple<std::string, std::string, int>> savegames;
};

#endif // _SAVEGAMES_H_