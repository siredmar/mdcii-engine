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

#include "savegames.hpp"
#include "files.hpp"
#include "gam/gam_parser.hpp"

Savegames::Savegames(const std::string& basepath, const std::string& file_ending)
{
  auto files = Files::instance();
  auto savegamefolder = files->find_path_for_file(basepath);
  auto tree = files->get_directories_files(savegamefolder);
  for (auto& s : tree)
  {
    if (s.find(file_ending) != std::string::npos)
    {
      // ignoring lastgame.gam file in list as this is a copy of one other savegame.
      if (s.find("lastgame.gam") == std::string::npos)
      {
        GamParser p(s, true);
        std::string gameName = files->instance()->get_file_name(s, false);
        savegames.push_back(std::tuple<std::string, std::string, int>(s, gameName, p.getSceneRanking()));
      }
    }
  }
}

unsigned int Savegames::size() const
{
  return savegames.size();
}

std::experimental::optional<std::string> Savegames::getPath(unsigned int index) const
{
  if (index < savegames.size())
  {
    return std::get<0>(savegames[index]);
  }
  return {};
}

std::experimental::optional<std::string> Savegames::getName(unsigned int index) const
{
  if (index < savegames.size())
  {
    return std::get<1>(savegames[index]);
  }
  return {};
}


std::experimental::optional<int> Savegames::getRanking(unsigned int index) const
{
  if (index < savegames.size())
  {
    return std::get<2>(savegames[index]);
  }
  return {};
}


std::vector<std::tuple<std::string, std::string, int>> Savegames::getSavegames() const
{
  return savegames;
}