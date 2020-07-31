
// This file is part of the MDCII Game Engine.
// Copyright (C) 2020 Armin Schlegel
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

#ifndef _BSHRESOURCES_HPP_
#define _BSHRESOURCES_HPP_

#include <map>
#include <memory>

#include "bsh/bshreader.hpp"
#include "bsh/zeireader.hpp"
#include "files/files.hpp"

class BshResources
{
public:
    static BshResources* CreateInstance();
    static BshResources* Instance();

    bool BshReaderExists(std::string key);
    std::shared_ptr<BshReader> GetBshReader(std::string key);
    bool ZeiReaderExists(std::string key);
    std::shared_ptr<ZeiReader> GetZeiReader(std::string key);

private:
    static BshResources* _instance;
    Files* files;
    std::map<std::string, std::shared_ptr<BshReader>> bshMap;
    std::map<std::string, std::shared_ptr<ZeiReader>> zeiMap;

    BshResources();
    BshResources(const BshResources&) = delete;

    class CGuard
    {
    public:
        ~CGuard()
        {
            if (NULL != BshResources::_instance)
            {
                delete BshResources::_instance;
                BshResources::_instance = NULL;
            }
        }
    };
};

#endif // _BSHRESOURCES_HPP_