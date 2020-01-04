
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

#ifndef FILES_HPP
#define FILES_HPP
#include <map>
#include <string>
#include <vector>

class Files
{
public:
  static Files* create_instance(std::string path);
  static Files* instance();
  void init();
  void init(std::string path);

  bool check_file(const std::string& filename);
  bool check_all_files(std::vector<std::string>* files);
  std::string find_path_for_file(std::string file);

private:
  static Files* _instance;
  ~Files() {}
  Files(std::string path) { init(path); }

  Files(const Files&);

  std::vector<std::string> get_directory_tree(const std::string& path);
  std::string string_to_lower_case(const std::string& str);

  std::vector<std::string> tree;

  class CGuard
  {
  public:
    ~CGuard()
    {
      if (NULL != Files::_instance)
      {
        delete Files::_instance;
        Files::_instance = NULL;
      }
    }
  };
};

#endif
