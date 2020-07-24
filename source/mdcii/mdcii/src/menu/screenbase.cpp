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

#include "menu/screenbase.hpp"

using namespace sdlgui;

ScreenBase::ScreenBase()
  : scale(Scale::Instance())
{
}

void ScreenBase::Redraw()
{
  for (auto& w : widgets)
  {
    auto widget = std::get<0>(w);
    int scaleLeftBorder = (scale->GetScreenSize().width - 1024) / 2;
    int scaleUpperBorder = (scale->GetScreenSize().height - 768) / 2;
    widget->setPosition(Vector2i{std::get<1>(w) + scaleLeftBorder, std::get<2>(w) + scaleUpperBorder});
  }
}

void ScreenBase::Handle()
{
  // implemented in inherited instances
}