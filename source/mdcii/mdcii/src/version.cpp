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

#include "version.hpp"
#include "bsh_leser.hpp"
#include "files.hpp"
#include <iostream>

Anno_version Version::Detect_game_version()
{
  auto files = Files::instance();
  Bsh_leser bsh(files->instance()->find_path_for_file("sgfx/stadtfld.bsh"));
  if (bsh.anzahl() == 5748)
  {
    std::cout << "[INFO] Vanilla Anno 1602 Installation erkannt" << std::endl;
    return Anno_version::VANILLA;
  }
  else // sgfx.anzahl == 5964
  {
    std::cout << "[INFO] NINA Anno 1602 Installation erkannt" << std::endl;
    return Anno_version::NINA;
  }
}
