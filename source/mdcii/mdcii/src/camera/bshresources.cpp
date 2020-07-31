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

#include <tuple>

#include "camera/bshresources.hpp"

extern std::vector<std::pair<std::string, std::string>> FilesToCheck;

BshResources* BshResources::_instance = 0;

BshResources* BshResources::Instance()
{
    if (not _instance)
    {
        throw("[EER] BshResources not initialized yet!");
    }
    return _instance;
}

BshResources* BshResources::CreateInstance()
{
    static CGuard g;
    if (!_instance)
    {
        _instance = new BshResources();
    }
    return _instance;
}

BshResources::BshResources()
    : files(Files::Instance())
{
    for (auto const& f : FilesToCheck)
    {
        auto file = std::get<0>(f);
        if (std::get<1>(f) == "bsh")
        {
            bshMap[file] = std::make_shared<BshReader>(files->FindPathForFile(file));
        }
        else
        {
            zeiMap[file] = std::make_shared<ZeiReader>(files->FindPathForFile(file));
        }
    }
}

bool BshResources::BshReaderExists(std::string key)
{
    if (bshMap.find(key) == bshMap.end())
    {
        return false;
    }
    return true;
}

std::shared_ptr<BshReader> BshResources::GetBshReader(std::string key)
{
    return bshMap[key];
}

bool BshResources::ZeiReaderExists(std::string key)
{
    if (zeiMap.find(key) == zeiMap.end())
    {
        return false;
    }
    return true;
}

std::shared_ptr<ZeiReader> BshResources::GetZeiReader(std::string key)
{
    return zeiMap[key];
}