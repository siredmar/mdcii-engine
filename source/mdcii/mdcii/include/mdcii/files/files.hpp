
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
  static Files* CreateInstance(const std::string& path);
  static Files* Instance();

  bool CheckFile(const std::string& filename);
  bool CheckAllFiles(std::vector<std::pair<std::string, std::string>>* files);
  std::string FindPathForFile(const std::string& file);
  std::vector<std::string> GetDirectoryTree(const std::string& path);
  std::vector<std::string> GetDirectoryFiles(const std::string& directory);
  std::vector<std::string> GrepFiles(const std::string& search);
  std::string StringToLowerCase(const std::string& str);
  static std::string GetFilename(const std::string& filePath, bool withExtention = true);

private:
  static Files* _instance;
  explicit Files(const std::string& path)
  {
    tree = GetDirectoryTree(path);
  }

  Files(const Files&) = delete;


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
