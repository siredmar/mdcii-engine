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

#ifndef FILES_TO_CHECK_HPP
#define FILES_TO_CHECK_HPP

#include <string>
#include <tuple>
#include <vector>

// clang-format off
std::vector<std::pair<std::string, std::string>> FilesToCheck = {
    {"sgfx/effekte.bsh", "bsh"},
    {"mgfx/effekte.bsh", "bsh"},
    {"gfx/effekte.bsh", "bsh"},
    {"sgfx/ship.bsh", "bsh"},
    {"mgfx/ship.bsh", "bsh"},
    {"gfx/ship.bsh", "bsh"},
    {"sgfx/soldat.bsh", "bsh"},
    {"mgfx/soldat.bsh", "bsh"},
    {"gfx/soldat.bsh", "bsh"},
    {"sgfx/stadtfld.bsh", "bsh"},
    {"mgfx/stadtfld.bsh", "bsh"},
    {"gfx/stadtfld.bsh", "bsh"},
    {"toolgfx/zei16g.zei", "zei"},
    //, "sgfx/numbers.bsh", "bsh"},
    //, "mgfx/numbers.bsh", "bsh"},
    //, "gfx/numbers.bsh", "bsh"},
    //, "sgfx/tiere.bsh", "bsh"},
    //, "mgfx/tiere.bsh", "bsh"},
    //, "gfx/tiere.bsh", "bsh"},
    //, "sgfx/traeger.bsh", "bsh"},
    //, "mgfx/traeger.bsh", "bsh"},
    //, "gfx/traeger.bsh", "bsh"},
    //, "sgfx/maeher.bsh", "bsh"},
    //, "mgfx/maeher.bsh", "bsh"},
    //, "gfx/maeher.bsh", "bsh"},
};
// clang-format on

#endif // FILES_TO_CHECK_HPP
