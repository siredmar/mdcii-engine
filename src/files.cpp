
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

Files* Files::create_instance(std::string path)
{
  static CGuard g;
  if (!_instance)
  {
    _instance = new Files(path);
  }
  return _instance;
}

void Files::init()
{
  // Defaults to current location
  init(".");
}

void Files::init(std::string path)
{
  tree = get_directory_tree(path);
}

bool Files::check_all_files(std::vector<std::string>* files)
{
  bool failed = false;
  for (auto const& f : *files)
  {
    std::cout << "[INFO] Checking for file: " << f << std::endl;
    if (check_file(find_path_for_file(f)) == false)
    {
      failed = true;
      std::cout << "[ERR] File not found: " << f << std::endl;
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
  for (auto t : tree)
  {
    std::string tree_file = string_to_lower_case(t);
    if (tree_file.find(boost::filesystem::path::preferred_separator + file) != std::string::npos)
    {
      std::cout << "[INFO] Path found [" << file << "]: " << t << std::endl;
      return t;
    }
  }
  return file;
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

bool Files::check_file(const std::string& filename)
{
  std::ifstream f(filename.c_str());
  return f.good();
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
