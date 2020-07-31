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

#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>

#include "files/files.hpp"
#include "framebuffer/palette.hpp"

Palette* Palette::_instance = 0;

Palette* Palette::Instance()
{
    if (not _instance)
    {
        throw("[ERR] Palette not initialized yet!");
    }
    return _instance;
}

Palette* Palette::CreateInstance(const std::string& paletteFilePath)
{
    static CGuard g;
    if (!_instance)
    {
        _instance = new Palette(paletteFilePath);
        _instance->transparentColor = _instance->FindTransparentColorIndex();
        _instance->c = (SDL_Color*)malloc(_instance->size() * sizeof(SDL_Color));
        for (int i = 0; i < _instance->size(); i++)
        {
            _instance->c[i].r = _instance->GetColor(i).getRed();
            _instance->c[i].g = _instance->GetColor(i).getGreen();
            _instance->c[i].b = _instance->GetColor(i).getBlue();
        }
    }
    return _instance;
}

Palette::~Palette()
{
    free(_instance->c);
}

Palette::Palette(const std::string& paletteFilePath)
{
    path = paletteFilePath;
    std::ifstream input(path, std::ios::binary);
    std::vector<uint8_t> buffer(std::istreambuf_iterator<char>(input), {});
    // 20 is the chunk definition int bytes to jump over
    for (unsigned int i = 20; i < buffer.size() - 4; i = i + 4)
    {
        PaletteColor color(static_cast<uint8_t>(buffer[i]), static_cast<uint8_t>(buffer[i + 1]), static_cast<uint8_t>(buffer[i + 2]));
        palette.push_back(color);
    }
}

SDL_Color* Palette::GetSDLColors()
{
    return c;
}

PaletteColor Palette::GetColor(int index)
{
    return palette.at(index);
}

int Palette::GetTransparentColor()
{
    return transparentColor;
}

int Palette::FindTransparentColorIndex()
{
    for (int i = palette.size(); i >= 0; i--)
    {
        // this is hard coded to magenta: 0xFF, 0x00, 0xFF
        if (palette[i].getRed() == 0xFF && palette[i].getGreen() == 0x00 && palette[i].getBlue() == 0xFF)
        {
            std::cout << "[INFO] transparent color palette index: " << i << std::endl;
            return i;
        }
    }
    std::cout << "[ERR] transparent color palette index not found" << std::endl;
    return -1;
}

int Palette::size()
{
    return (int)palette.size();
}

uint8_t Palette::Index(int index)
{
    if (index / 3 >= size())
    {
        return 0;
    }
    if (index % 3 == 0)
    {
        return GetColor(index / 3).getRed();
    }
    else if (index % 3 == 1)
    {
        return GetColor(index / 3).getGreen();
    }
    else
    {
        return GetColor(index / 3).getBlue();
    }
}
