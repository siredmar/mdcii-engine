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

#include "menu/scale.hpp"

Scale* Scale::_instance = 0;

Scale* Scale::CreateInstance(SDL_Window* window)
{
    static CGuard g;
    if (not _instance)
    {
        _instance = new Scale(window);
    }
    return _instance;
}

Scale* Scale::Instance()
{
    if (not _instance)
    {
        throw("[EER] Scale not initialized yet!");
    }
    return _instance;
}

Scale::ScreenSize Scale::GetScreenSize()
{
    int width;
    int height;
    SDL_GetWindowSize(window, &width, &height);
    return ScreenSize(width, height);
}

int Scale::GetScreenWidth()
{
    int width;
    int height;
    SDL_GetWindowSize(window, &width, &height);
    return width;
}

int Scale::GetScreenHeight()
{
    int width;
    int height;
    SDL_GetWindowSize(window, &width, &height);
    return height;
}

void Scale::Update()
{
    SetScreenSize(GetScreenSize());
}

void Scale::SetScreenSize(Scale::ScreenSize newSize)
{
    SDL_SetWindowSize(window, newSize.width, newSize.height);
}

void Scale::SetFullscreen(bool enabled)
{
    SDL_SetWindowFullscreen(window, static_cast<int>(enabled));
}

void Scale::ToggleFullscreen()
{
    fullscreen = !fullscreen;
    SDL_SetWindowFullscreen(window, static_cast<int>(fullscreen));
}

Scale::Scale(SDL_Window* window)
    : window(window)
{
}
