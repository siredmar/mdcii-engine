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

Savegames::Savegames()
{
  auto files = Files::instance();
  auto savegamefolder = files->find_path_for_file("savegame");
  auto tree = files->get_directories_files(savegamefolder);
  for (auto& s : tree)
  {
    if (s.find(".gam") != std::string::npos)
    {
      savegames.push_back(s);
    }
  }
}

int Savegames::size() const
{
  return (int)savegames.size();
}

std::experimental::optional<std::string> Savegames::getPath(int index) const
{
  if (index < savegames.size())
  {
    return savegames[index];
  }
  return {};
}

std::vector<std::string> Savegames::getSavegames() const
{
  return savegames;
}