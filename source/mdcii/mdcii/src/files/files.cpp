
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
#include <fstream>
#include <iostream>

#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>

#include "files/files.hpp"

using namespace boost::filesystem;

Files* Files::_instance = 0;

Files* Files::Instance()
{
    if (not _instance)
    {
        throw("[EER] Files not initialized yet!");
    }
    return _instance;
}

Files* Files::CreateInstance(const std::string& path)
{
    static CGuard g;
    if (!_instance)
    {
        _instance = new Files(path);
    }
    return _instance;
}

bool Files::CheckAllFiles(std::vector<std::pair<std::string, std::string>>* files)
{
    bool failed = false;
    for (auto const& f : *files)
    {
        std::cout << "[INFO] Checking for file: " << std::get<0>(f) << std::endl;
        if (CheckFile(FindPathForFile(std::get<0>(f))) == false)
        {
            failed = true;
            std::cout << "[ERR] File not found: " << std::get<0>(f) << std::endl;
        }
    }
    if (failed == true)
    {
        return false;
    }
    return true;
}

std::string Files::FindPathForFile(const std::string& file)
{
    std::string lcaseFile = StringToLowerCase(file);
    // Search for the file as substring in the lowercased directory tree
    for (auto t : tree)
    {
        std::string tree_file = StringToLowerCase(t);
        if (tree_file.find(lcaseFile) != std::string::npos)
        {
            std::cout << "[INFO] Path found [" << file << "]: " << t << std::endl;
            return t;
        }
    }
    throw("[ERR] cannot find file: " + file);
    return "";
}

std::string Files::StringToLowerCase(const std::string& str)
{
    std::string modified_str = str;

    for (auto& c : modified_str)
    {
        c = std::tolower(c);
    }
    return modified_str;
}

bool Files::CheckFile(const std::string& filename)
{
    std::ifstream f(filename.c_str());
    return f.good();
}

std::vector<std::string> Files::GetDirectoryTree(const std::string& path)
{
    std::vector<std::string> tree;
    boost::filesystem::symlink_option options = boost::filesystem::symlink_option::recurse;
    for (auto& p : boost::filesystem::recursive_directory_iterator(path, options))
    {
        tree.push_back(p.path().string());
    }
    return tree;
}

std::vector<std::string> Files::GetDirectoryFiles(const std::string& directory)
{
    std::vector<std::string> files;
    path p(directory);
    if (is_directory(p))
    {
        std::cout << p << " is a directory containing:\n";

        for (auto& entry : boost::make_iterator_range(directory_iterator(p), {}))
        {
            files.push_back(entry.path().string());
        }
    }
    return files;
}

std::vector<std::string> Files::GrepFiles(const std::string& search)
{
    std::vector<std::string> list;
    std::string lcaseSearch = StringToLowerCase(search);
    // Search for the file as substring in the lowercased directory tree
    for (auto t : tree)
    {
        std::string tree_file = StringToLowerCase(t);
        if (tree_file.find(lcaseSearch) != std::string::npos)
        {
            list.push_back(t);
        }
    }
    return list;
}

std::string Files::GetFilename(const std::string& filePath, bool withExtention)
{
    boost::filesystem::path p(filePath);

    if (withExtention == false)
    {
        if (p.has_stem())
        {
            return p.stem().string();
        }
        return "";
    }
    else
    {
        return p.filename().string();
    }
}