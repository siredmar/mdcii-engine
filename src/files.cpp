
// This file is part of the MDCII Game Engine.
// Copyright (C) 2019  Armin Schlegel
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

#include <fstream>
#include <iostream>
#include <vector>
#include <filesystem>
#include <algorithm>

#include "files.hpp"

std::map<std::string, std::string> files = {
  {"sgfx_effekte_bsh", "sgfx/effekte.bsh"},
  {"mgfx_effekte_bsh", "mgfx/effekte.bsh"},
  {"gfx_effekte_bsh", "gfx/effekte.bsh"},

  {"sgfx_ship_bsh", "sgfx/ship.bsh"},
  {"mgfx_ship_bsh", "mgfx/ship.bsh"},
  {"gfx_ship_bsh", "gfx/ship.bsh"},

  {"sgfx_soldat_bsh", "sgfx/soldat.bsh"},
  {"mgfx_soldat_bsh", "mgfx/soldat.bsh"},
  {"gfx_soldat_bsh", "gfx/soldat.bsh"},

  {"sgfx_stadtfld_bsh", "sgfx/stadtfld.bsh"},
  {"mgfx_stadtfld_bsh", "mgfx/stadtfld.bsh"},
  {"gfx_stadtfld_bsh", "gfx/stadtfld.bsh"},

  {"toolgfx_zeig16g_zei", "toolgfx/zei16g.zei"},

  {"grafiken_txt", "grafiken.txt"},
  {"bebauung_txt", "bebauung.txt"},

  // those are not used at the moment
  // {"sgfx_numbers_bsh", "sgfx/numbers.bsh"},
  // {"mgfx_numbers_bsh", "mgfx/numbers.bsh"},
  // {"gfx_numbers_bsh", "gfx/numbers.bsh"},

  // {"sgfx_tiere_bsh", "sgfx/tiere.bsh"},
  // {"mgfx_tiere_bsh", "mgfx/tiere.bsh"},
  // {"gfx_tiere_bsh", "gfx/tiere.bsh"},

  // {"sgfx_traeger_bsh", "sgfx/traeger.bsh"},
  // {"mgfx_traeger_bsh", "mgfx/traeger.bsh"},
  // {"gfx_traeger_bsh", "gfx/traeger.bsh"},

  // {"sgfx_maeher_bsh", "sgfx/maeher.bsh"},
  // {"mgfx_maeher_bsh", "mgfx/maeher.bsh"},
  // {"gfx_maeher_bsh", "gfx/maeher.bsh"},
};

bool check_file(const std::string &filename)
{
  std::ifstream f(filename.c_str());
  return f.good();
}

bool check_all_files(std::map<std::string, std::string> files) 
{
  bool failed = false;
  for (auto const &f: files) 
  {
    std::cout << "[INFO] Checking for file: " << f.second << std::endl;
    if(check_file(f.second) == false)
    {
      failed = true;
      std::cout << "[ERR] File not found: " << f.second << std::endl; 
    }
  }
  if (failed == true) 
  {
      return false;
  }
  return true;
}

std::vector<std::string> get_directory_tree(const std::string& path) 
{
  std::vector<std::string> tree;
  std::filesystem::directory_options options = std::filesystem::directory_options::follow_directory_symlink;
  for(auto& p: std::filesystem::recursive_directory_iterator(path, options))
  {
    tree.push_back(p.path());
  }
  return tree;
}

std::string string_to_lower_case(const std::string& str)
{
  std::locale loc;
  std::string modified_str = str;

  for(auto& c : modified_str)
  {
    c = std::tolower(c);
  }
  return modified_str;
}

std::map<std::string, std::string> create_file_map(const std::string& path, std::map<std::string, std::string> map)
{
  auto tree = get_directory_tree(path);
  std::map<std::string, std::string> modified_map = map;

  for(auto e: map)
  {
    // Search for the file as substring in the lowercased directory tree 
    for(auto f: tree)
    {
      std::string file = string_to_lower_case(f);
      if (file.find(e.second) != std::string::npos) {
        modified_map[e.first] = f;
        break; 
      } 
    }
  }
  return modified_map;
}