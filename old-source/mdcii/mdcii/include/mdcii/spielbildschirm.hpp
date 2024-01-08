
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

#ifndef SPIELBILDSCHIRM_HPP
#define SPIELBILDSCHIRM_HPP

#include <memory>

#include "camera/camera.hpp"
#include "framebuffer/framebuffer.hpp"
#include "karte.hpp"
#include "version/version.hpp"
#include "welt.hpp"

class Spielbildschirm
{
public:
    explicit Spielbildschirm(Framebuffer& fb);
    ~Spielbildschirm();
    void zeichne_bild(Welt& welt, int maus_x, int maus_y);

    std::shared_ptr<Kamera> kamera; // vorübergehend public

private:
    Spielbildschirm() = default;
    Framebuffer& fb;
    Karte karte;
    std::shared_ptr<Buildings> buildings;
};

#endif
