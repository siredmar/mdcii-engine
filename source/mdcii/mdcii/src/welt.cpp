
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

#include "welt.hpp"
#include "files.hpp"
#include <string.h>

Welt::Welt(std::istream& f, std::shared_ptr<Haeuser> haeuser)
  : haeuser(haeuser)
  , ani(0)
{
  auto files = Files::instance();

  while (!f.eof())
  {
    bloecke.push_back(new Block(f));
  }
  std::vector<Block*>::iterator i = bloecke.begin();
  while (i < bloecke.end())
  {
    if (strcmp((*i)->kennung, Insel5::kennung) == 0)
    {
      inseln.push_back(new Insel(*i, *(i + 1), haeuser));
      i++;
    }
    else if (strcmp((*i)->kennung, Kontor::kennung) == 0)
    {
      for (int j = 0; j < (*i)->laenge / sizeof(Kontor); j++)
        kontore.push_back((Kontor&)(*i)->daten[j * sizeof(Kontor)]);
    }
    else if (strcmp((*i)->kennung, Ship::kennung) == 0)
    {
      for (int j = 0; j < (*i)->laenge / sizeof(Ship); j++)
        schiffe.push_back((Ship&)(*i)->daten[j * sizeof(Ship)]);
    }
    else if (strcmp((*i)->kennung, Soldat::kennung) == 0)
    {
      for (int j = 0; j < (*i)->laenge / sizeof(Soldat); j++)
        soldaten.push_back((Soldat&)(*i)->daten[j * sizeof(Soldat)]);
    }
    else if (strcmp((*i)->kennung, Prodlist::kennung) == 0)
    {
      for (int j = 0; j < (*i)->laenge / sizeof(Prodlist); j++)
        prodlist.push_back((Prodlist&)(*i)->daten[j * sizeof(Prodlist)]);
    }
    else if (strcmp((*i)->kennung, Player::kennung) == 0)
    {
      for (int j = 0; j < (*i)->laenge / sizeof(Player); j++)
        spieler.push_back((Player&)(*i)->daten[j * sizeof(Player)]);
    }
    i++;
  }

  // Initialisiere Animationen über Gebäuden

  for (auto& prod : prodlist)
  {
    int x = prod.x_pos + inseln[prod.inselnummer]->xpos;
    int y = prod.y_pos + inseln[prod.inselnummer]->ypos;
    inselfeld_t inselfeld;
    feld_an_pos(inselfeld, x, y);
    auto info = haeuser->get_haus(inselfeld.bebauung);
    if (info)
    {
      int max_x = (((inselfeld.rot & 1) == 0) ? info.value()->Size.w : info.value()->Size.h) - 1;
      int max_y = (((inselfeld.rot & 1) == 0) ? info.value()->Size.h : info.value()->Size.w) - 1;
      if (info.value()->HAUS_PRODTYP.Kind == ProdtypKindType::HANDWERK)
      {
        int versatz = (info.value()->Size.w + info.value()->Size.h) / 2;
        versatz += (versatz & 1) * 2;
        if (!((prod.modus & 1) != 0)) // Betrieb ist geschlossen
        {
          animationen[std::pair<int, int>(x, y)] = {x * 256 + max_x * 128, y * 256 + max_y * 128, 256 + versatz * 205, 0, 350, 32, (max_x + max_y) * 128, true};
        }
        if ((prod.ani & 0x0f) == 0x0f) // Betrieb hat Rohstoffmangel
        {
          animationen[std::pair<int, int>(x, y)] = {x * 256 + max_x * 128, y * 256 + max_y * 128, 256 + versatz * 205, 0, 382, 32, (max_x + max_y) * 128, true};
        }
      }
    }
  }

  // Initialisiere Animationen über Bergen
  for (Insel*& insel : inseln)
  {
    for (int i = 0; i < reinterpret_cast<Insel5*>(insel->inselX->daten)->erzvorkommen; i++)
    {
      const Erzvorkommen& erz = reinterpret_cast<Insel5*>(insel->inselX->daten)->erze[i];
      int x = erz.x_pos + insel->xpos;
      int y = erz.y_pos + insel->ypos;
      inselfeld_t inselfeld;
      feld_an_pos(inselfeld, x, y);
      auto info = haeuser->get_haus(inselfeld.bebauung);
      if (info)
      {
        int max_x = (((inselfeld.rot & 1) == 0) ? info.value()->Size.w : info.value()->Size.h) - 1;
        int max_y = (((inselfeld.rot & 1) == 0) ? info.value()->Size.h : info.value()->Size.w) - 1;
        if (info.value()->Kind == ObjectKindType::FELS)
        {
          int versatz = (info.value()->Size.h + info.value()->Size.w) / 2;
          versatz += (versatz & 1) * 2 + 3;
          if (erz.typ == 2) // Eisen
          {
            animationen[std::pair<int, int>(x, y)]
                = {x * 256 + max_x * 128, y * 256 + max_y * 128, 256 + versatz * 205, 0, 556, 32, (max_x + max_y) * 128, true};
          }
          if (erz.typ == 3) // Gold
          {
            animationen[std::pair<int, int>(x, y)]
                = {x * 256 + max_x * 128, y * 256 + max_y * 128, 256 + versatz * 205, 0, 588, 32, (max_x + max_y) * 128, true};
          }
        }
      }
    }
  }
}

int Welt::inselnummer_an_pos(uint16_t x, uint16_t y)
{
  for (int i = 0; i < inseln.size(); i++)
  {
    if ((x >= inseln[i]->xpos) && (y >= inseln[i]->ypos) && (x < inseln[i]->xpos + inseln[i]->breite) && (y < inseln[i]->ypos + inseln[i]->hoehe))
      return i;
  }
  return -1;
}

Insel* Welt::insel_an_pos(uint16_t x, uint16_t y)
{
  for (Insel* insel : inseln)
  {
    if ((x >= insel->xpos) && (y >= insel->ypos) && (x < insel->xpos + insel->breite) && (y < insel->ypos + insel->hoehe))
      return insel;
  }
  return NULL;
}

void Welt::simulationsschritt()
{
  ani = (ani + 1) % haeuser->get_haus(1201).value()->AnimAnz;
  for (Insel* insel : inseln)
  {
    insel->bewege_wasser();
  }
  for (Prodlist& prod : prodlist)
  {
    if (prod.modus & 1 && prod.rohstoff1_menge >= 16 && prod.produkt_menge < 320)
      inseln[prod.inselnummer]->animiere_gebaeude(prod.x_pos, prod.y_pos);
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
    auto info = haeuser->get_haus(feld.bebauung);
    feld.ani = (0x80000000 + y + x * 3 + ani) % info.value()->AnimAnz;
  }
}

Prodlist* Welt::prodlist_an_pos(uint8_t insel, uint8_t x, uint8_t y)
{
  // Provisorium! Muss viel effizienter werden!
  for (int i = 0; i < prodlist.size(); i++)
  {
    if (prodlist[i].inselnummer == insel && prodlist[i].x_pos == x && prodlist[i].y_pos == y)
      return &prodlist[i];
  }
  return nullptr;
}

Ship* Welt::schiff_an_pos(uint16_t x, uint16_t y)
{
  // Provisorium! Muss viel effizienter werden!
  for (int i = 0; i < schiffe.size(); i++)
  {
    if (schiffe[i].x_pos == x && schiffe[i].y_pos == y)
      return &schiffe[i];
  }
  return nullptr;
}

uint8_t Welt::spielerfarbe(uint8_t spieler)
{
  return this->spieler[spieler].farbe;
}
