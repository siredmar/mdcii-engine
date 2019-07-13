
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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
// USA.

#include <algorithm>
#include <boost/filesystem.hpp>
#include <fstream>
#include <iostream>

#include "files.hpp"

Files* Files::_instance = 0;

Files* Files::instance()
{
  return _instance;
}

Files* Files::create_instance(std::string path, bool check_files)
{
  static CGuard g;
  if (!_instance)
  {
    _instance = new Files(path, check_files);
  }
  return _instance;
}

void Files::init()
{
  // Defaults to current location
  init(".", false);
}

void Files::init(std::string path, bool check_files)
{
  files = {
      {"sgfx/effekte.bsh", "sgfx/effekte.bsh"},

      {"mgfx/effekte.bsh", "mgfx/effekte.bsh"},

      {"gfx/effekte.bsh", "gfx/effekte.bsh"},

      {"sgfx/ship.bsh", "sgfx/ship.bsh"},

      {"mgfx/ship.bsh", "mgfx/ship.bsh"},

      {"gfx/ship.bsh", "gfx/ship.bsh"},

      {"sgfx/soldat.bsh", "sgfx/soldat.bsh"},

      {"mgfx/soldat.bsh", "mgfx/soldat.bsh"},

      {"gfx/soldat.bsh", "gfx/soldat.bsh"},

      {"sgfx/stadtfld.bsh", "sgfx/stadtfld.bsh"},

      {"mgfx/stadtfld.bsh", "mgfx/stadtfld.bsh"},

      {"gfx/stadtfld.bsh", "gfx/stadtfld.bsh"},

      {"toolgfx/zeig16g_zei", "toolgfx/zei16g.zei"},

      {"grafiken.txt", "grafiken.txt"},

      {"bebauung.txt", "bebauung.txt"},

      // those are not used at the moment
      // {"sgfx/numbers.bsh", "sgfx/numbers.bsh"},
      // {"mgfx/numbers.bsh", "mgfx/numbers.bsh"},
      // {"gfx/numbers.bsh", "gfx/numbers.bsh"},

      // {"sgfx/tiere.bsh", "sgfx/tiere.bsh"},
      // {"mgfx/tiere.bsh", "mgfx/tiere.bsh"},
      // {"gfx/tiere.bsh", "gfx/tiere.bsh"},

      // {"sgfx/traeger.bsh", "sgfx/traeger.bsh"},
      // {"mgfx/traeger.bsh", "mgfx/traeger.bsh"},
      // {"gfx/traeger.bsh", "gfx/traeger.bsh"},

      // {"sgfx/maeher.bsh", "sgfx/maeher.bsh"},
      // {"mgfx/maeher.bsh", "mgfx/maeher.bsh"},
      // {"gfx/maeher.bsh", "gfx/maeher.bsh"},
  };
  tree = get_directory_tree(path);
  if (check_files == true)
  {
    files = create_file_map(path, files);
  }
}
bool Files::check_file(const std::string& filename)
{
  std::ifstream f(filename.c_str());
  return f.good();
}

bool Files::check_all_files()
{
  bool failed = false;
  for (auto const& f : files)
  {
    std::cout << "[INFO] Checking for file: " << f.second << std::endl;
    if (check_file(f.second) == false)
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

std::string Files::find_path_for_file(std::string file)
{
  // Search for the file as substring in the lowercased directory tree
  std::cout << "[INFO] Searching path for file: " << file << std::endl;
  for (auto t : tree)
  {
    std::string tree_file = string_to_lower_case(t);
    if (tree_file.find(file) != std::string::npos)
    {
      std::cout << "[INFO] Path found [" << file << "]: " << t << std::endl;
      return t;
    }
  }
  return file;
}

std::map<std::string, std::string> Files::create_file_map(const std::string& path, std::map<std::string, std::string> map)
{
  std::map<std::string, std::string> modified_map = map;

  for (auto e : map)
  {
    modified_map[e.first] = find_path_for_file(e.second);
  }
  return modified_map;
}

std::string Files::get_file(std::string key)
{
  std::cout << "Loading: files[" << key << "]: " << files[key] << std::endl;
  return files[key];
}

std::string Files::string_to_lower_case(const std::string& str)
{
  std::locale loc;
  std::string modified_str = str;

  for (auto& c : modified_str)
  {
    c = std::tolower(c);
  }
  return modified_str;
}

std::vector<std::string> Files::get_directory_tree(const std::string& path)
{
  std::vector<std::string> tree;
  boost::filesystem::symlink_option options = boost::filesystem::symlink_option::recurse;
  for (auto& p : boost::filesystem::recursive_directory_iterator(path, options))
  {
    tree.push_back(p.path().string());
  }
  return tree;
}