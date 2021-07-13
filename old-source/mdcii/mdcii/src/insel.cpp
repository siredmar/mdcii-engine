
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

#include "insel.hpp"
#include "cod/buildings.hpp"
#include "files/files.hpp"
#include "strukturen.hpp"
#include <boost/format.hpp>
#include <fstream>
#include <iostream>
#include <string.h>
#include <string>

void Insel::insel_rastern(inselfeld_t* a, uint32_t length, inselfeld_t* b, uint8_t width, uint8_t height)
{
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            b[y * width + x].bebauung = 0xffff;
        }
    }

    for (unsigned int i = 0; i < length; i++)
    {
        inselfeld_t& feld = a[i];

        if (feld.x_pos >= width || feld.y_pos >= height)
            continue;

        auto info = buildings->GetHouse(feld.bebauung);
        if (info)
        {
            int buildingWidth;
            int buildingHeight;
            if (feld.rot % 2 == 0)
            {
                buildingWidth = info.value()->Size.w;
                buildingHeight = info.value()->Size.h;
            }
            else
            {
                buildingHeight = info.value()->Size.h;
                buildingWidth = info.value()->Size.w;
            }
            for (int y = 0; y < buildingHeight && feld.y_pos + y < height; y++)
            {
                for (int x = 0; x < buildingWidth && feld.x_pos + x < width; x++)
                {
                    b[(feld.y_pos + y) * width + feld.x_pos + x] = feld;
                    b[(feld.y_pos + y) * width + feld.x_pos + x].x_pos = x;
                    b[(feld.y_pos + y) * width + feld.x_pos + x].y_pos = y;
                }
            }
        }
        else
        {
            b[feld.y_pos * width + feld.x_pos] = feld;
            b[feld.y_pos * width + feld.x_pos].x_pos = 0;
            b[feld.y_pos * width + feld.x_pos].y_pos = 0;
        }
    }
}

/*30x30 "lit%02d.scp"
40x40 "mit%02d.scp"
50x52 "med%02d.scp"
70x60 "big%02d.scp"
100x90 "lar%02d.scp"
*/

std::string Insel::basisname(uint8_t width, uint8_t num, uint8_t sued)
{
    // 30x30
    if (width < 35)
        return boost::str(boost::format("%slit%02d.scp") % (sued ? "sued/" : "nord/") % (int)num);
    // 40x40
    if (width < 45)
        return boost::str(boost::format("%smit%02d.scp") % (sued ? "sued/" : "nord/") % (int)num);
    // 50x52
    if (width < 60)
        return boost::str(boost::format("%smed%02d.scp") % (sued ? "sued/" : "nord/") % (int)num);
    // 70x60
    if (width < 85)
        return boost::str(boost::format("%sbig%02d.scp") % (sued ? "sued/" : "nord/") % (int)num);
    // 100x90
    return boost::str(boost::format("%slar%02d.scp") % (sued ? "sued/" : "nord/") % (int)num);
}

Insel::Insel(Block* inselX, Block* inselhaus, std::shared_ptr<Buildings> buildings)
    : buildings(buildings)
{
    // Kennung prüfen
    if (strcmp(inselX->kennung, Insel5::kennung) == 0)
    {
        this->inselX = inselX;
        this->width = ((Insel5*)inselX->data)->width;
        this->height = ((Insel5*)inselX->data)->height;
        this->xpos = ((Insel5*)inselX->data)->x_pos;
        this->ypos = ((Insel5*)inselX->data)->y_pos;
        this->schicht1 = new inselfeld_t[this->width * this->height];
        this->schicht2 = new inselfeld_t[this->width * this->height];
        if (((Insel5*)inselX->data)->diff == 0)
        {
            std::ifstream f;
            auto files = Files::Instance();
            std::string karte = this->basisname(this->width, ((Insel5*)inselX->data)->basis, ((Insel5*)inselX->data)->sued).c_str();
            karte = files->FindPathForFile(karte);
            if (files->CheckFile(karte) == false)
            {
                std::cout << "[ERR] Island not found: " << karte << std::endl;
                exit(EXIT_FAILURE);
            }

            f.open(karte, std::ios_base::in | std::ios_base::binary);
            // The first read is the inselX_basis. It is not needed here. Therefore read it and throw it away
            (void)Block(f);
            Block inselhaus_basis = Block(f);
            this->insel_rastern((inselfeld_t*)inselhaus_basis.data, inselhaus_basis.length / 8, schicht1, this->width, this->height);
            f.close();
        }

        this->insel_rastern((inselfeld_t*)inselhaus->data, inselhaus->length / 8, schicht2, this->width, this->height);

        uint8_t x, y;
        for (y = 0; y < this->height; y++)
        {
            for (x = 0; x < this->width; x++)
            {
                if (schicht2[y * this->width + x].bebauung == 0xffff)
                    schicht2[y * this->width + x] = schicht1[y * this->width + x];
            }
        }
    }
    else if (strcmp(inselX->kennung, Insel3::kennung) == 0)
    {
        this->inselX = inselX;
        this->width = ((Insel3*)inselX->data)->width;
        this->height = ((Insel3*)inselX->data)->height;
        this->xpos = ((Insel3*)inselX->data)->x_pos;
        this->ypos = ((Insel3*)inselX->data)->y_pos;
        this->schicht1 = new inselfeld_t[this->width * this->height];
        this->schicht2 = new inselfeld_t[this->width * this->height];
        this->insel_rastern((inselfeld_t*)inselhaus->data, inselhaus->length / 8, schicht1, this->width, this->height);
        memcpy(schicht2, schicht1, sizeof(inselfeld_t) * this->width * this->height);
    }
}

// unnused grafik_boden
void Insel::grafik_boden(feld_t& target, uint8_t x, uint8_t y, [[maybe_unused]] uint8_t r)
{
    auto info = buildings->GetHouse(schicht2[y * width + x].bebauung);
    if (info)
    {
        int grafik = info.value()->Gfx;
        if (schicht2[y * width + x].bebauung != 0xffff)
        {
            if ((info.value()->Posoffs == 0 ? 0 : 1) == 0 && grafik != -1)
            {
                target.index = grafik;
                target.grundhoehe = info.value()->Posoffs == 0 ? 0 : 1;
                return;
            }
        }
        if (schicht1[y * width + x].bebauung != 0xffff)
        {
            if (info.value()->Highflg == 0 && grafik != -1)
            {
                target.index = grafik;
                target.grundhoehe = info.value()->Posoffs == 0 ? 0 : 1;
                return;
            }
        }
    }
    target.index = 0;
    target.grundhoehe = 1;
}

void Insel::inselfeld_bebauung(inselfeld_t& target, uint8_t x, uint8_t y)
{
    uint8_t xp = schicht2[y * width + x].x_pos;
    uint8_t yp = schicht2[y * width + x].y_pos;
    if ((yp > y) || (xp > x))
    {
        // TODO "target" auf einen sinnvollen Wert setzen
        return;
    }
    target = schicht2[(y - yp) * width + x - xp];
    target.x_pos = xp;
    target.y_pos = yp;
}

void Insel::grafik_bebauung_inselfeld(feld_t& target, inselfeld_t& feld, uint8_t r, std::shared_ptr<Buildings> buildings)
{
    if (feld.bebauung == 0xffff)
    {
        target.index = -1;
        target.grundhoehe = 0;
        return;
    }
    auto info = buildings->GetHouse(feld.bebauung);

    if (!info || info.value()->Gfx == -1)
    {
        target.index = -1;
        target.grundhoehe = 0;
        return;
    }
    int grafik = info.value()->Gfx;
    int index = grafik;
    int richtungen = 1;
    if (info.value()->Rotate > 0)
    {
        richtungen = 4;
    }
    int ani_schritte = 1;
    if (info.value()->AnimAnz > 0)
    {
        ani_schritte = info.value()->AnimAnz;
    }
    index += info.value()->Rotate * ((r + feld.rot) % richtungen);
    switch (feld.rot)
    {
        case 0:
            index += feld.y_pos * info.value()->Size.w + feld.x_pos;
            break;
        case 1:
            index += (info.value()->Size.h - feld.x_pos - 1) * info.value()->Size.w + feld.y_pos;
            break;
        case 2:
            index += (info.value()->Size.h - feld.y_pos - 1) * info.value()->Size.w + (info.value()->Size.w - feld.x_pos - 1);
            break;
        case 3:
            index += feld.x_pos * info.value()->Size.w + (info.value()->Size.w - feld.y_pos - 1);
            break;
        default:
            break;
    }
    index += info.value()->Size.h * info.value()->Size.w * richtungen * (feld.ani % ani_schritte);
    target.index = index;
    int grundhoehe = 0;
    if (info.value()->Posoffs == 20)
    {
        grundhoehe = 1;
    }
    target.grundhoehe = grundhoehe;
}

// unused grafik_bebauung
void Insel::grafik_bebauung(feld_t& target, uint8_t x, uint8_t y, uint8_t r)
{
    inselfeld_t feld;
    inselfeld_bebauung(feld, x, y);
    grafik_bebauung_inselfeld(target, feld, r, buildings);
}

void Insel::bewege_wasser()
{
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            inselfeld_t& feld = schicht2[y * width + x];
            if (((feld.bebauung >= 1201) && (feld.bebauung <= 1218)) || ((feld.bebauung >= 901) && (feld.bebauung <= 905))
                || ((feld.bebauung >= 1251) && (feld.bebauung <= 1259)) || ((feld.bebauung == 1071) || (feld.bebauung == 2311)))
            {
                auto info = buildings->GetHouse(feld.bebauung);
                if (info)
                {
                    if (info.value()->AnimAnz > 0)
                    {
                        feld.ani = (feld.ani + 1) % (info.value()->AnimAnz);
                    }
                }
            }
        }
    }
}

void Insel::animiere_gebaeude(uint8_t x, uint8_t y)
{
    inselfeld_t& feld = schicht2[y * width + x];
    auto info = buildings->GetHouse(feld.bebauung);
    if (info)
    {
        if (info.value()->AnimTime != -1)
        {
            if (info.value()->AnimAnz > 0)
            {
                feld.ani = (feld.ani + 1) % (info.value()->AnimAnz);
            }
        }
    }
}
