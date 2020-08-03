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

#include "cod/text_cod.hpp"

TextCod* TextCod::_instance = 0;

TextCod* TextCod::CreateInstance(const std::string& path, bool decode)
{
    static CGuard g;
    if (!_instance)
    {
        _instance = new TextCod(path, decode);
    }
    return _instance;
}

TextCod* TextCod::CreateInstance(const std::string& fileAsString)
{
    static CGuard g;
    if (!_instance)
    {
        _instance = new TextCod(fileAsString);
    }
    return _instance;
}

TextCod* TextCod::Instance()
{
    if (not _instance)
    {
        throw("[EER] TextCod not initialized yet!");
    }
    return _instance;
}

TextCod::TextCod(const std::string& file, bool decode)
{
    codTxt = ReadFile(file, decode, false);
    Parse();
}

TextCod::TextCod(const std::string& fileAsString)
{
    codTxt = ReadFileAsString(fileAsString, false);
    Parse();
}

TextcodPb::Section* TextCod::GetSection(const std::string& name)
{
    if (!textMap.count(name))
    {
        throw std::string("[ERR] TextCod::GetSection " + name + " section does not exist");
    }
    return textMap[name];
}

int TextCod::GetSectionSize(const std::string& name)
{
    if (!textMap.count(name))
    {
        throw std::string("[ERR] TextCod::GetSection " + name + " section does not exist");
    }
    return textMap[name]->value_size();
}

std::string TextCod::GetValue(const std::string& section, int index)
{
    auto s = GetSection(section);
    if (index >= s->value_size())
    {
        throw std::string("[ERR] TextCod::GetValue index out of boundaries");
    }
    return textMap[section]->value(index);
}

void TextCod::Parse()
{
    for (auto& line : codTxt)
    {
        // find the begin of a new section and add a new group
        if (IsSubstring(line, "[END]") || IsSubstring(line, "--------------------------------------------------"))
        {
            continue;
        }
        if (IsSubstring(line, "["))
        {
            auto name = RemoveLeadingCharacters(line, '[');
            name = RemoveTrailingCharacters(name, ']');
            auto section = texts.add_section();
            section->set_name(name);
            currentSection = section;
            textMap[name] = section;
        }
        // this is a real value that gets stored in the current group
        else
        {
            currentSection->add_value(line.c_str());
        }
    }
}