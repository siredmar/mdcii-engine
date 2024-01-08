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

#include <iostream>

#include "bsh/bshreader.hpp"
#include "files/files.hpp"
#include "version/version.hpp"

AnnoVersion Version::DetectGameVersion()
{
    try
    {
        auto files = Files::Instance();
        BshReader bsh(files->FindPathForFile("sgfx/stadtfld.bsh"));
        if (bsh.Count() == 5748)
        {
            return AnnoVersion::VANILLA;
        }
        else // sgfx.count == 5964
        {
            return AnnoVersion::NINA;
        }
    }
    catch (...)
    {
        return AnnoVersion::OTHER;
    }
}

std::string Version::GameVersionString()
{
    auto version = DetectGameVersion();
    if (version == AnnoVersion::VANILLA)
    {
        return "vanilla";
    }
    else if (version == AnnoVersion::NINA)
    {
        return "NINA";
    }
    else
    {
        return "OTHER";
    }
}