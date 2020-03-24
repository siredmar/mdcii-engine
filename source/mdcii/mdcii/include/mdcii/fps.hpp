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

// Based on nanogui-sdl by Dalerank <dalerankn8@gmail.com>
#ifndef FPS_H_
#define FPS_H_
#include <SDL2/SDL.h>

class Fps
{
public:
  explicit Fps(int tickInterval = 41)
    : m_tickInterval(tickInterval)
    , m_nextTime(SDL_GetTicks() + tickInterval)
  {
  }

  void next()
  {
    SDL_Delay(getTicksToNextFrame());

    m_nextTime += m_tickInterval;
  }

private:
  const int m_tickInterval;
  Uint32 m_nextTime;

  Uint32 getTicksToNextFrame() const
  {
    Uint32 now = SDL_GetTicks();

    return (m_nextTime <= now) ? 0 : m_nextTime - now;
  }
};

#endif