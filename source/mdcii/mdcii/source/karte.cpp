
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

#include "karte.hpp"

Karte::Karte(int xpos, int ypos, int breite, int hoehe)
{
  this->xpos = xpos;
  this->ypos = ypos;
  this->breite = breite;
  this->hoehe = hoehe;
}

void Karte::zeichne_bild(Bildspeicher& bs, Welt& welt)
{
  for (Insel* insel : welt.inseln)
  {
    for (int y = 0; y < insel->hoehe; y++)
    {
      for (int x = 0; x < insel->breite; x++)
      {
        inselfeld_t feld;
        insel->inselfeld_bebauung(feld, x, y);
        uint8_t farbe = 0;
        uint8_t nummer = (feld.spieler < 4) ? welt.spielerfarbe(feld.spieler) : feld.spieler;
        switch (nummer)
        {
          case 0: farbe = 183; break; // rot
          case 1: farbe = 97; break;  // blau
          case 2: farbe = 71; break;  // gelb
          case 3: farbe = 7; break;   // grau
          case 6: farbe = 2; break;   // Eingeborene
          case 7: farbe = 182; break; // frei
        }
        // FIXME: Ineffizient! Zu viele Divisionen
        if (!(feld.bebauung >= 1201 && feld.bebauung <= 1221 || feld.bebauung >= 1251 && feld.bebauung <= 1259))
          bs.zeichne_pixel(xpos + (insel->xpos + x) * breite / Welt::KARTENBREITE, ypos + (insel->ypos + y) * hoehe / Welt::KARTENHOEHE, farbe);
      }
    }
  }

  for (Ship& schiff : welt.schiffe)
  {
    int x = xpos + schiff.x_pos * breite / Welt::KARTENBREITE;
    int y = ypos + schiff.y_pos * hoehe / Welt::KARTENHOEHE;
    bs.zeichne_rechteck(x, y, x + 1, y + 1, 252);
  }
}

void Karte::zeichne_kameraposition(Bildspeicher& bs, Kamera& kamera)
{
  int x00, y00, x01, y01, x10, y10, x11, y11;

  kamera.auf_karte(bs, 0, 0, x00, y00);
  kamera.auf_karte(bs, bs.breite, 0, x01, y01);
  kamera.auf_karte(bs, 0, bs.hoehe, x10, y10);
  kamera.auf_karte(bs, bs.breite, bs.hoehe, x11, y11);

  x00 = xpos + x00 * breite / Welt::KARTENBREITE;
  y00 = ypos + y00 * hoehe / Welt::KARTENHOEHE;
  x01 = xpos + x01 * breite / Welt::KARTENBREITE;
  y01 = ypos + y01 * hoehe / Welt::KARTENHOEHE;
  x10 = xpos + x10 * breite / Welt::KARTENBREITE;
  y10 = ypos + y10 * hoehe / Welt::KARTENHOEHE;
  x11 = xpos + x11 * breite / Welt::KARTENBREITE;
  y11 = ypos + y11 * hoehe / Welt::KARTENHOEHE;

  bs.zeichne_linie(x00, y00 + 1, x01, y01 + 1, 252, 0x33);
  bs.zeichne_linie(x01, y01 + 1, x11, y11 + 1, 252, 0x33);
  bs.zeichne_linie(x00, y00 + 1, x10, y10 + 1, 252, 0x33);
  bs.zeichne_linie(x10, y10 + 1, x11, y11 + 1, 252, 0x33);

  bs.zeichne_linie(x00, y00, x01, y01, 255, 0x33);
  bs.zeichne_linie(x01, y01, x11, y11, 255, 0x33);
  bs.zeichne_linie(x00, y00, x10, y10, 255, 0x33);
  bs.zeichne_linie(x10, y10, x11, y11, 255, 0x33);
}
