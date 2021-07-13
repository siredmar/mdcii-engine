
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

#include <iostream>
#include <string.h>

#include <SDL2/SDL.h>

#include "files/files.hpp"
#include "welt.hpp"

Welt::Welt(std::istream& f)
    : ani(0)
{
    buildings = Buildings::Instance();
    while (!f.eof())
    {
        try
        {
            bloecke.push_back(new Block(f));
        }
        catch (const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
    }
    std::vector<Block*>::iterator i = bloecke.begin();
    while (i < bloecke.end())
    {
        if (strcmp((*i)->kennung, Insel5::kennung) == 0)
        {
            if ((strcmp((*(i + 1))->kennung, inselhaus_kennung) == 0) && (strcmp((*(i + 2))->kennung, inselhaus_kennung) == 0))
            {
                inseln.push_back(new Insel(*i, *(i + 2), buildings));
                i = i + 2;
            }
            else
            {
                inseln.push_back(new Insel(*i, *(i + 1), buildings));
                ++i;
            }
        }
        else if (strcmp((*i)->kennung, Kontor::kennung) == 0)
        {
            for (unsigned int j = 0; j < (*i)->length / sizeof(Kontor); j++)
                kontore.push_back((Kontor&)(*i)->data[j * sizeof(Kontor)]);
        }
        else if (strcmp((*i)->kennung, Ship::kennung) == 0)
        {
            for (unsigned int j = 0; j < (*i)->length / sizeof(Ship); j++)
                schiffe.push_back((Ship&)(*i)->data[j * sizeof(Ship)]);
        }
        else if (strcmp((*i)->kennung, Soldat::kennung) == 0)
        {
            for (unsigned int j = 0; j < (*i)->length / sizeof(Soldat); j++)
                soldaten.push_back((Soldat&)(*i)->data[j * sizeof(Soldat)]);
        }
        else if (strcmp((*i)->kennung, Prodlist::kennung) == 0)
        {
            for (unsigned int j = 0; j < (*i)->length / sizeof(Prodlist); j++)
                prodlist.push_back((Prodlist&)(*i)->data[j * sizeof(Prodlist)]);
        }
        else if (strcmp((*i)->kennung, Player::kennung) == 0)
        {
            for (unsigned int j = 0; j < (*i)->length / sizeof(Player); j++)
                spieler.push_back((Player&)(*i)->data[j * sizeof(Player)]);
        }
        ++i;
    }

    // Initialisiere Animationen über Gebäuden

    for (auto& prod : prodlist)
    {
        try
        {
            if (prod.inselnummer <= inseln.size())
            {
                int x = prod.x_pos + inseln[prod.inselnummer]->xpos;
                int y = prod.y_pos + inseln[prod.inselnummer]->ypos;
                inselfeld_t inselfeld;
                feld_an_pos(inselfeld, x, y);
                auto info = buildings->GetHouse(inselfeld.bebauung);
                if (info)
                {
                    int max_x = (((inselfeld.rot & 1) == 0) ? info.value()->Size.w : info.value()->Size.h) - 1;
                    int max_y = (((inselfeld.rot & 1) == 0) ? info.value()->Size.h : info.value()->Size.w) - 1;
                    if (info.value()->HouseProductionType.Kind == ProdtypKindType::HANDWERK)
                    {
                        int offset = (info.value()->Size.w + info.value()->Size.h) / 2;
                        offset += (offset & 1) * 2;
                        if (!((prod.modus & 1) != 0)) // Betrieb ist geschlossen
                        {
                            animationen[std::pair<int, int>(x, y)]
                                = { x * 256 + max_x * 128, y * 256 + max_y * 128, 256 + offset * 205, 0, 350, 32, (max_x + max_y) * 128, true };
                        }
                        if ((prod.ani & 0x0f) == 0x0f) // Betrieb hat Rohstoffmangel
                        {
                            animationen[std::pair<int, int>(x, y)]
                                = { x * 256 + max_x * 128, y * 256 + max_y * 128, 256 + offset * 205, 0, 382, 32, (max_x + max_y) * 128, true };
                        }
                    }
                }
            }
        }
        catch (std::exception& ex)
        {
            std::cout << ex.what() << std::endl;
        }
    }

    // Initialisiere Animationen über Bergen
    for (Insel*& insel : inseln)
    {
        for (int i = 0; i < reinterpret_cast<Insel5*>(insel->inselX->data)->erzvorkommen; i++)
        {
            const Erzvorkommen& erz = reinterpret_cast<Insel5*>(insel->inselX->data)->erze[i];
            int x = erz.x_pos + insel->xpos;
            int y = erz.y_pos + insel->ypos;
            inselfeld_t inselfeld;
            feld_an_pos(inselfeld, x, y);
            auto info = buildings->GetHouse(inselfeld.bebauung);
            if (info)
            {
                int max_x = (((inselfeld.rot & 1) == 0) ? info.value()->Size.w : info.value()->Size.h) - 1;
                int max_y = (((inselfeld.rot & 1) == 0) ? info.value()->Size.h : info.value()->Size.w) - 1;
                if (info.value()->Kind == ObjectKindType::FELS)
                {
                    int offset = (info.value()->Size.h + info.value()->Size.w) / 2;
                    offset += (offset & 1) * 2 + 3;
                    if (erz.typ == 2) // Eisen
                    {
                        animationen[std::pair<int, int>(x, y)]
                            = { x * 256 + max_x * 128, y * 256 + max_y * 128, 256 + offset * 205, 0, 556, 32, (max_x + max_y) * 128, true };
                    }
                    if (erz.typ == 3) // Gold
                    {
                        animationen[std::pair<int, int>(x, y)]
                            = { x * 256 + max_x * 128, y * 256 + max_y * 128, 256 + offset * 205, 0, 588, 32, (max_x + max_y) * 128, true };
                    }
                }
            }
        }
    }
}

int Welt::inselnummer_an_pos(uint16_t x, uint16_t y)
{
    for (unsigned int i = 0; i < inseln.size(); i++)
    {
        if ((x >= inseln[i]->xpos) && (y >= inseln[i]->ypos) && (x < inseln[i]->xpos + inseln[i]->width) && (y < inseln[i]->ypos + inseln[i]->height))
            return i;
    }
    return -1;
}

Insel* Welt::insel_an_pos(uint16_t x, uint16_t y)
{
    for (Insel* insel : inseln)
    {
        if ((x >= insel->xpos) && (y >= insel->ypos) && (x < insel->xpos + insel->width) && (y < insel->ypos + insel->height))
            return insel;
    }
    return NULL;
}

void Welt::simulationsschritt()
{
    uint32_t now = SDL_GetTicks();
    static uint32_t old = 0;
    int tickdiff = static_cast<int>(now - old);
    auto water = buildings->GetHouse(1201);
    if (water)
    {
        if (tickdiff >= water.value()->AnimTime)
        {
            ani = (ani + 1) % water.value()->AnimAnz;
            for (Insel* insel : inseln)
            {
                insel->bewege_wasser();
            }
            old = now;
        }
    }
    for (Prodlist& prod : prodlist)
    {
        if (prod.inselnummer <= inseln.size())
        {
            if (prod.modus & 1 && prod.rohstoff1_menge >= 16 && prod.produkt_menge < 320)
                inseln[prod.inselnummer]->animiere_gebaeude(prod.x_pos, prod.y_pos);
        }
    }
    for (auto& map_elem : animationen)
    {
        Animation& animation = map_elem.second;
        if (animation.ani != -1)
        {
            if (animation.ani < animation.schritte - 1)
            {
                animation.ani++;
            }
            else
            {
                if (animation.wiederholen)
                    animation.ani = 0;
                else
                    animation.ani = -1;
            }
        }
    }
}

void Welt::feld_an_pos(inselfeld_t& feld, int x, int y)
{
    Insel* insel = insel_an_pos(x, y);
    if (insel != NULL)
        insel->inselfeld_bebauung(feld, x - insel->xpos, y - insel->ypos);
    else
    {
        memset(&feld, 0, sizeof(inselfeld_t));
        feld.bebauung = 1201;
        auto info = buildings->GetHouse(feld.bebauung);
        if (info)
        {
            feld.ani = (0x80000000 + y + x * 3 + ani) % info.value()->AnimAnz;
        }
    }
}

Prodlist* Welt::prodlist_an_pos(uint8_t insel, uint8_t x, uint8_t y)
{
    // Provisorium! Muss viel effizienter werden!
    for (unsigned int i = 0; i < prodlist.size(); i++)
    {
        if (prodlist[i].inselnummer == insel && prodlist[i].x_pos == x && prodlist[i].y_pos == y)
            return &prodlist[i];
    }
    return nullptr;
}

Ship* Welt::schiff_an_pos(uint16_t x, uint16_t y)
{
    // Provisorium! Muss viel effizienter werden!
    for (unsigned int i = 0; i < schiffe.size(); i++)
    {
        if (schiffe[i].x_pos == x && schiffe[i].y_pos == y)
            return &schiffe[i];
    }
    return nullptr;
}

uint8_t Welt::spielerfarbe(uint8_t spieler)
{
    return this->spieler[spieler].color;
}
