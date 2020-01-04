
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

#include <string>
#include "kamera.hpp"
#include "files.hpp"
#include "haeuser.hpp"

const int Kamera::x_raster[3] = {8, 16, 32};
const int Kamera::y_raster[3] = {4, 8, 16};
const int Kamera::grundhoehe[3] = {5, 10, 20};

Kamera::Kamera(std::shared_ptr<Haeuser> haeuser)
  : haeuser(haeuser)
{
  xpos = Welt::KARTENBREITE / 2;
  ypos = Welt::KARTENHOEHE / 2;
  drehung = 0;
  vergroesserung = 1;
  auto files = Files::instance();
  effekte_bsh[0] = new Bsh_leser(files->instance()->find_path_for_file("sgfx/effekte.bsh"));
  effekte_bsh[1] = new Bsh_leser(files->instance()->find_path_for_file("mgfx/effekte.bsh"));
  effekte_bsh[2] = new Bsh_leser(files->instance()->find_path_for_file("gfx/effekte.bsh"));

  // maeher_bsh[0] = new Bsh_leser(files->instance()->find_path_for_file("sgfx/maeher.bsh"));
  // maeher_bsh[1] = new Bsh_leser(files->instance()->find_path_for_file("mgfx/maeher.bsh"));
  // maeher_bsh[2] = new Bsh_leser(files->instance()->find_path_for_file("gfx/maeher.bsh"));

  // numbers_bsh[0] = new Bsh_leser(files->instance()->find_path_for_file("sgfx/numbers.bsh"));
  // numbers_bsh[1] = new Bsh_leser(files->instance()->find_path_for_file("mgfx/numbers.bsh"));
  // numbers_bsh[2] = new Bsh_leser(files->instance()->find_path_for_file("gfx/numbers.bsh"));

  ship_bsh[0] = new Bsh_leser(files->instance()->find_path_for_file("sgfx/ship.bsh"));
  ship_bsh[1] = new Bsh_leser(files->instance()->find_path_for_file("mgfx/ship.bsh"));
  ship_bsh[2] = new Bsh_leser(files->instance()->find_path_for_file("gfx/ship.bsh"));

  soldat_bsh[0] = new Bsh_leser(files->instance()->find_path_for_file("sgfx/soldat.bsh"));
  soldat_bsh[1] = new Bsh_leser(files->instance()->find_path_for_file("mgfx/soldat.bsh"));
  soldat_bsh[2] = new Bsh_leser(files->instance()->find_path_for_file("gfx/soldat.bsh"));

  stadtfld_bsh[0] = new Bsh_leser(files->instance()->find_path_for_file("sgfx/stadtfld.bsh"));
  stadtfld_bsh[1] = new Bsh_leser(files->instance()->find_path_for_file("mgfx/stadtfld.bsh"));
  stadtfld_bsh[2] = new Bsh_leser(files->instance()->find_path_for_file("gfx/stadtfld.bsh"));

  // tiere_bsh[0] = new Bsh_leser(files->instance()->find_path_for_file("sgfx/tiere.bsh"));
  // tiere_bsh[1] = new Bsh_leser(files->instance()->find_path_for_file("mgfx/tiere.bsh"));
  // tiere_bsh[2] = new Bsh_leser(files->instance()->find_path_for_file("gfx/tiere.bsh"));

  // traeger_bsh[0] = new Bsh_leser(files->instance()->find_path_for_file("sgfx/traeger.bsh"));
  // traeger_bsh[1] = new Bsh_leser(files->instance()->find_path_for_file("mgfx/traeger.bsh"));
  // traeger_bsh[2] = new Bsh_leser(files->instance()->find_path_for_file("gfx/traeger.bsh"));

  zei = new Zei_leser(files->instance()->find_path_for_file("toolgfx/zei16g.zei"));
}

void Kamera::gehe_zu(uint16_t x, uint16_t y)
{
  xpos = (x < 0) ? 0 : ((x >= Welt::KARTENBREITE) ? Welt::KARTENBREITE - 1 : x);
  ypos = (y < 0) ? 0 : ((y >= Welt::KARTENHOEHE) ? Welt::KARTENHOEHE - 1 : y);
}

void Kamera::nach_links()
{
  switch (drehung)
  {
    case 0:
      if (xpos > 0)
        xpos--;
      if (ypos < Welt::KARTENHOEHE - 1)
        ypos++;
      break;
    case 1:
      if (xpos < Welt::KARTENBREITE - 1)
        xpos++;
      if (ypos < Welt::KARTENHOEHE - 1)
        ypos++;
      break;
    case 2:
      if (xpos < Welt::KARTENBREITE - 1)
        xpos++;
      if (ypos > 0)
        ypos--;
      break;
    case 3:
      if (xpos > 0)
        xpos--;
      if (ypos > 0)
        ypos--;
      break;
  }
}

void Kamera::nach_rechts()
{
  switch (drehung)
  {
    case 0:
      if (xpos < Welt::KARTENBREITE - 1)
        xpos++;
      if (ypos > 0)
        ypos--;
      break;
    case 1:
      if (xpos > 0)
        xpos--;
      if (ypos > 0)
        ypos--;
      break;
    case 2:
      if (xpos > 0)
        xpos--;
      if (ypos < Welt::KARTENHOEHE - 1)
        ypos++;
      break;
    case 3:
      if (xpos < Welt::KARTENBREITE - 1)
        xpos++;
      if (ypos < Welt::KARTENHOEHE - 1)
        ypos++;
      break;
  }
}

void Kamera::nach_oben()
{
  switch (drehung)
  {
    case 0:
      if (xpos > 0)
        xpos--;
      if (ypos > 0)
        ypos--;
      break;
    case 1:
      if (xpos > 0)
        xpos--;
      if (ypos < Welt::KARTENHOEHE - 1)
        ypos++;
      break;
    case 2:
      if (xpos < Welt::KARTENBREITE - 1)
        xpos++;
      if (ypos < Welt::KARTENHOEHE - 1)
        ypos++;
      break;
    case 3:
      if (xpos < Welt::KARTENBREITE - 1)
        xpos++;
      if (ypos > 0)
        ypos--;
      break;
  }
}

void Kamera::nach_unten()
{
  switch (drehung)
  {
    case 0:
      if (xpos < Welt::KARTENBREITE - 1)
        xpos++;
      if (ypos < Welt::KARTENHOEHE - 1)
        ypos++;
      break;
    case 1:
      if (xpos < Welt::KARTENBREITE - 1)
        xpos++;
      if (ypos > 0)
        ypos--;
      break;
    case 2:
      if (xpos > 0)
        xpos--;
      if (ypos > 0)
        ypos--;
      break;
    case 3:
      if (xpos > 0)
        xpos--;
      if (ypos < Welt::KARTENHOEHE - 1)
        ypos++;
      break;
  }
}

void Kamera::vergroessern()
{
  if (vergroesserung < 2)
    vergroesserung++;
}

void Kamera::verkleinern()
{
  if (vergroesserung > 0)
    vergroesserung--;
}

void Kamera::setze_vergroesserung(uint8_t vergroesserung)
{
  this->vergroesserung = vergroesserung;
}

void Kamera::links_drehen()
{
  drehung = (drehung + 3) % 4;
}

void Kamera::rechts_drehen()
{
  drehung = (drehung + 1) % 4;
}

void Kamera::auf_bildschirm(Bildspeicher& bs, int karte_x, int karte_y, int& bildschirm_x, int& bildschirm_y)
{
  int x = karte_x - xpos;
  int y = karte_y - ypos;
  switch (drehung)
  {
    case 0:
      bildschirm_x = bs.breite / 2 + (x - y - 1) * x_raster[vergroesserung];
      bildschirm_y = bs.hoehe / 2 + (x + y) * y_raster[vergroesserung];
      break;
    case 1: // TODO: Fälle 1-3 überprüfen
      bildschirm_x = bs.breite / 2 + (-x - y - 1) * x_raster[vergroesserung];
      bildschirm_y = bs.hoehe / 2 + (x + -y) * y_raster[vergroesserung];
      break;
    case 2:
      bildschirm_x = bs.breite / 2 - (x - y - 1) * x_raster[vergroesserung];
      bildschirm_y = bs.hoehe / 2 - (x + y) * y_raster[vergroesserung];
      break;
    case 3:
      bildschirm_x = bs.breite / 2 - (-x - y - 1) * x_raster[vergroesserung];
      bildschirm_y = bs.hoehe / 2 - (x + -y) * y_raster[vergroesserung];
      break;
  }
}

void Kamera::auf_bildschirm(Bildspeicher& bs, int karte_x, int karte_y, int karte_z, int& bildschirm_x, int& bildschirm_y, int& bildschirm_z)
{
  auf_bildschirm(bs, karte_x, karte_y, bildschirm_x, bildschirm_y);
  int x = karte_x - xpos;
  int y = karte_y - ypos;
  switch (drehung)
  {
    case 0: bildschirm_z = x + y; break;
    case 1: bildschirm_z = x - y; break;
    case 2: bildschirm_z = -(x + y); break;
    case 3: bildschirm_z = -(x - y); break;
  }
  bildschirm_y -= karte_z * grundhoehe[vergroesserung];
  bildschirm_z *= 256;
}

void Kamera::auf_bildschirm_256(Bildspeicher& bs, int karte_x, int karte_y, int karte_z, int& bildschirm_x, int& bildschirm_y, int& bildschirm_z)
{
  int x = karte_x - 256 * xpos;
  int y = karte_y - 256 * ypos;
  switch (drehung)
  {
    case 0:
      bildschirm_x = bs.breite / 2 + (x - y - 256) * x_raster[vergroesserung] / 256;
      bildschirm_y = bs.hoehe / 2 + (x + y) * y_raster[vergroesserung] / 256;
      bildschirm_z = x + y;
      break;
    case 1: // TODO: Fälle 1-3 überprüfen
      bildschirm_x = bs.breite / 2 + (-x - y - 256) * x_raster[vergroesserung] / 256;
      bildschirm_y = bs.hoehe / 2 + (x + -y) * y_raster[vergroesserung] / 256;
      bildschirm_z = x - y;
      break;
    case 2:
      bildschirm_x = bs.breite / 2 - (x - y - 256) * x_raster[vergroesserung] / 256;
      bildschirm_y = bs.hoehe / 2 - (x + y) * y_raster[vergroesserung] / 256;
      bildschirm_z = -(x + y);
      break;
    case 3:
      bildschirm_x = bs.breite / 2 - (-x - y - 256) * x_raster[vergroesserung] / 256;
      bildschirm_y = bs.hoehe / 2 - (x + -y) * y_raster[vergroesserung] / 256;
      bildschirm_z = -(x - y);
      break;
  }
  bildschirm_y -= karte_z * grundhoehe[vergroesserung] / 256;
}

void Kamera::auf_karte(Bildspeicher& bs, int bildschirm_x, int bildschirm_y, int& karte_x, int& karte_y)
{
  int temp1 = (bildschirm_x - (int)bs.breite / 2) / x_raster[vergroesserung];
  int temp2 = (bildschirm_y - (int)bs.hoehe / 2) / y_raster[vergroesserung];
  int temp3 = (temp2 - temp1 + 1) / 2;
  switch (drehung)
  {
    case 0:
      karte_x = temp1 + temp3 + xpos;
      karte_y = temp3 + ypos;
      break;
    case 1: // TODO: Fälle 1-3 überprüfen
      karte_x = temp3 + xpos;
      karte_y = -(temp1 + temp3) + ypos;
      break;
    case 2:
      karte_x = -(temp1 + temp3) + xpos;
      karte_y = -temp3 + ypos;
      break;
    case 3:
      karte_x = -temp3 + xpos;
      karte_y = temp1 + temp3 + ypos;
      break;
  }
}


struct bild_mit_pos_t
{
  Bsh_bild* bild;
  int bs_x;
  int bs_y;
  int bs_z;
  int x;
  int y;
  bool sp;
};


void Kamera::zeichne_bild(Bildspeicher& bs, Welt& welt, int maus_x, int maus_y)
{
  bool feld_unter_maus = false;
  int feld_unter_maus_x, feld_unter_maus_y;

  int karte_x, karte_y;
  auf_karte(bs, -x_raster[vergroesserung], -2 * y_raster[vergroesserung], karte_x, karte_y);
  int bildschirm_x, bildschirm_y;
  auf_bildschirm(bs, karte_x, karte_y, bildschirm_x, bildschirm_y);
  int felder_horizontal = bs.breite / x_raster[vergroesserung] / 2 + 2;
  int felder_vertikal = bs.hoehe / y_raster[vergroesserung] / 2 + 8;

  int ausdehnung = (felder_horizontal + felder_vertikal) / 2;

  std::vector<bild_mit_pos_t> felder;
  for (int y = ypos - ausdehnung; y < ypos + ausdehnung; y++)
  {
    for (int x = xpos - ausdehnung; x < xpos + ausdehnung; x++)
    {
      inselfeld_t inselfeld;
      welt.feld_an_pos(inselfeld, x, y);
      feld_t feld;
      Insel::grafik_bebauung_inselfeld(feld, inselfeld, drehung, haeuser);
      if (feld.index != -1)
      {
        int bs_x, bs_y, bs_z;
        auf_bildschirm(bs, x, y, feld.grundhoehe, bs_x, bs_y, bs_z);
        bild_mit_pos_t bild_mit_pos = {&stadtfld_bsh[vergroesserung]->gib_bsh_bild(feld.index), bs_x, bs_y + y_raster[vergroesserung], bs_z, x, y, true};
        felder.push_back(bild_mit_pos);
      }
    }
  }


  for (auto& map_elem : welt.animationen)
  {
    Animation& animation = map_elem.second;
    int bs_x, bs_y, bs_z;
    auf_bildschirm_256(bs, animation.x, animation.y, animation.z, bs_x, bs_y, bs_z);
    bild_mit_pos_t bild_mit_pos = {&effekte_bsh[vergroesserung]->gib_bsh_bild(animation.start_index + animation.ani), bs_x, bs_y, bs_z + animation.bs_z_versatz,
        map_elem.first.first, map_elem.first.second, false};
    felder.push_back(bild_mit_pos);
  }

  for (Ship& schiff : welt.schiffe)
  {
    int index;
    switch (schiff.typ)
    {
      case 0x15: index = 0; break;  // kleines Handelsschiff
      case 0x17: index = 32; break; // großes Handelsschiff
      case 0x1b: index = 48; break; // großes Kriegsschiff
      case 0x1d: index = 16; break; // fliegender Händler
      case 0x19: index = 64; break; // kleines Kriegsschiff
      case 0x1f: index = 80; break; // Piratenschiff     TODO: fahrender Händler (0x25)
      default: index = 0;
    }
    int bs_x, bs_y, bs_z;
    auf_bildschirm(bs, schiff.x_pos, schiff.y_pos, 0, bs_x, bs_y, bs_z);
    // zeichne Bugwellen
    if ((schiff.kurs_ziel & 0xff) != 0) // FIXME: Bedeutet dies wirklich, dass das Schiff fährt?
    {
      Bsh_bild& wellen = ship_bsh[vergroesserung]->gib_bsh_bild(12 * schiff.richtung + 96);
      felder.push_back({&wellen, bs_x, bs_y + y_raster[vergroesserung] * 2, bs_z + 512, schiff.x_pos, schiff.y_pos, false});
    }
    // zeichne Schiff
    Bsh_bild& bsh = ship_bsh[vergroesserung]->gib_bsh_bild(index + schiff.richtung); // FIXME
    felder.push_back({&bsh, bs_x, bs_y + y_raster[vergroesserung], bs_z + 512, schiff.x_pos, schiff.y_pos, false});
    // zeichne Flagge
    if (schiff.spieler != 4)
    {
      uint8_t nummer = (schiff.spieler < 4) ? welt.spielerfarbe(schiff.spieler) : schiff.spieler;
      Bsh_bild& flagge = ship_bsh[vergroesserung]->gib_bsh_bild(192 + nummer * 8);
      felder.push_back({&flagge, bs_x, bs_y + y_raster[vergroesserung], bs_z + 512, schiff.x_pos, schiff.y_pos, false});
    }
  }

  for (Soldat& soldat : welt.soldaten)
  {
    int index;
    switch (soldat.typ)
    {
      case 1: index = 0; break;     // Infanterist, rot
      case 2: index = 280; break;   // Infanterist, blau
      case 3: index = 560; break;   // Infanterist, gelb
      case 4: index = 840; break;   // Infanterist, grau
      case 5: index = 1120; break;  // Kavallerist, rot
      case 6: index = 1424; break;  // Kavallerist, blau
      case 7: index = 1728; break;  // Kavallerist, gelb
      case 8: index = 2032; break;  // Kavallerist, grau
      case 9: index = 3200; break;  // Musketier, rot
      case 10: index = 3336; break; // Musketier, blau
      case 11: index = 3472; break; // Musketier, gelb
      case 12: index = 3608; break; // Musketier, grau
      case 13: index = 2336; break; // Kanonier, rot
      case 14: index = 2552; break; // Kanonier, blau
      case 15: index = 2768; break; // Kanonier, gelb
      case 16: index = 2984; break; // Kanonier, grau
      case 33: index = 3744; break; // Eingeborener
      default: index = 0;
    }
    Bsh_bild& bsh = soldat_bsh[vergroesserung]->gib_bsh_bild(index + soldat.richtung * 8); // FIXME
    int bs_x, bs_y, bs_z;
    auf_bildschirm_256(bs, soldat.x_pos_2 * 128, soldat.y_pos_2 * 128, 256, bs_x, bs_y, bs_z);
    felder.push_back({&bsh, bs_x, bs_y + y_raster[vergroesserung], bs_z + 256, soldat.x_pos_2 / 2, soldat.y_pos_2 / 2, false});
  }

  std::stable_sort(felder.begin(), felder.end(), [](const bild_mit_pos_t& a, const bild_mit_pos_t& b) { return a.bs_z < b.bs_z; });

  for (bild_mit_pos_t& feld : felder)
  {
    bool schnitt;
    bs.zeichne_bsh_bild_sp_oz(*feld.bild, feld.bs_x, feld.bs_y, maus_x, maus_y, schnitt);
    if (schnitt && feld.sp)
    {
      feld_unter_maus_x = feld.x;
      feld_unter_maus_y = feld.y;
      feld_unter_maus = true;
    }
  }

  bs.setze_schriftfarbe(245, 0);
  bs.zeichne_string(*zei, "aktuelle Position:", 10, 10);
  bs.zeichne_string(*zei, "(" + std::to_string(xpos) + ", " + std::to_string(ypos) + ")", 10, 30);
  if (feld_unter_maus)
  {
    inselfeld_t inselfeld;
    welt.feld_an_pos(inselfeld, feld_unter_maus_x, feld_unter_maus_y);
    bs.zeichne_string(*zei, "Bebauung unter Mauszeiger:", 10, 60);
    bs.zeichne_string(*zei, std::to_string(inselfeld.bebauung), 10, 80);
    bs.zeichne_string(*zei, "Position und Insel unter Mauszeiger:", 10, 110);
    bs.zeichne_string(*zei,
        "(" + std::to_string(feld_unter_maus_x) + ", " + std::to_string(feld_unter_maus_y) + ")  Insel "
            + std::to_string(welt.inselnummer_an_pos(feld_unter_maus_x, feld_unter_maus_y)),
        10, 130);
  }
}
