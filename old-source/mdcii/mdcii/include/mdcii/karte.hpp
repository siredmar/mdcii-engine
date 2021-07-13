
// This file is part of the MDCII Game Engine.
// Copyright (C) 2015  Benedikt Freisen
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

#ifndef KARTE_HPP
#define KARTE_HPP

#include "camera/camera.hpp"
#include "framebuffer/framebuffer.hpp"
#include "welt.hpp"
#include <inttypes.h>

class Karte
{
    int xpos;
    int ypos;
    int width;
    int height;

public:
    explicit Karte(int xpos = 0, int ypos = 0, int width = 500, int height = 350);

    void zeichne_bild(Framebuffer& fb, Welt& welt);
    void zeichne_kameraposition(Framebuffer& fb, Kamera& kamera);
};

#endif
