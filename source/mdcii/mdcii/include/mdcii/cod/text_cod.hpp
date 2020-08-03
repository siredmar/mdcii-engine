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

#ifndef _TEXTCOD_HPP_
#define _TEXTCOD_HPP_

#include <map>
#include <string>
#include <vector>

#include "textcod.pb.h"

#include "cod/codhelpers.hpp"

class TextCod
{
public:
    explicit TextCod(const std::string& file, bool decode);
    explicit TextCod(const std::string& fileAsString);
    TextcodPb::Section* GetSection(const std::string& name);
    int GetSectionSize(const std::string& name);
    std::string GetValue(const std::string& section, int index);

private:
    void Parse();
    std::vector<std::string> codTxt;
    TextcodPb::Texts texts;
    TextcodPb::Section* currentSection;
    std::map<std::string, TextcodPb::Section*> textMap;
};

#endif // _TEXTCOD_HPP_
;