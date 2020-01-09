
// This file is part of the MDCII Game Engine.
// Copyright (C) 2019  Armin Schlegel
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

#ifndef HAEUSER_HPP
#define HAEUSER_HPP

#include <experimental/optional>
#include <map>

#include "cod_parser.hpp"

enum class ObjectKindType
{
  UNSET = 0,
  WALD,
  TOR,
  RUINE,
  HQ,
  STRANDMUND,
  STRANDHAUS,
  BRANDUNG,
  STRANDRUINE,
  HANGECK,
  TURM,
  STRANDECKI,
  STRANDVARI,
  STRANDECKA,
  MEER,
  BODEN,
  HANGQUELL,
  MAUER,
  FELS,
  MUENDUNG,
  HANG,
  MINE,
  BRANDECK,
  TURMSTRAND,
  PIER,
  STRAND,
  BRUECKE,
  FLUSSECK,
  FLUSS,
  HAFEN,
  STRASSE,
  GEBAEUDE,
  MAUERSTRAND,
  PLATZ
};

enum class RuinenrType
{
  UNSET = 0,
  RUINE_KONTOR_N1,
  RUINE_KONTOR_N2,
  RUINE_HOLZ,
  RUINE_KONTOR_1,
  RUINE_FELD,
  RUINE_MARKT,
  RUINE_STEIN,
  RUINE_MINE,
  RUINE_ROAD_STEIN,
  RUINE_ROAD_FELD
};

enum class BausampleType
{
  UNSET = 0,
  WAV_TRIUMPH,
  WAV_SCHWERTBAU,
  WAV_PYRAMIDE,
  WAV_KATHETRALE,
  WAV_NIX,
  WAV_SCHLOSS,
  WAV_KAPELLE,
  WAV_WOHNHAUS,
  WAV_BRUNNEN,
  WAV_KONTOR,
  WAV_GIESSEREI,
  WAV_FLEISCHER,
  WAV_SCHULE,
  WAV_DENKMAL,
  WAV_BAECKER,
  WAV_MINE,
  WAV_STANDARD,
  WAV_BURG,
  WAV_RUMBRENNER,
  WAV_PLANTAGE,
  WAV_ARZT,
  WAV_MUEHLE,
  WAV_PIRATEN,
  WAV_JAGDHUETTE,
  WAV_KIRCHE,
  WAV_MARKT,
  WAV_BADEHAUS,
  WAV_WIRTSHAUS,
  WAV_THEATER
};

enum class WorkstoffType
{
  UNSET = 0,
  HOLZ,
  STOFFE
};

enum class ErzbergnrType
{
  UNSET = 0,
  ERZBERG_KLEIN,
  ERZBERG_GROSS
};

enum class ProdtypKindType
{
  UNSET = 0,
  BRUNNEN,
  KONTOR,
  HOCHSCHULE,
  JAGDHAUS,
  STEINBRUCH,
  MARKT,
  PLANTAGE,
  DENKMAL,
  FISCHEREI,
  WEIDETIER,
  SCHLOSS,
  WIRT,
  HANDWERK,
  WACHTURM,
  KAPELLE,
  WERFT,
  THEATER,
  GALGEN,
  UNUSED,
  pMAUER,
  WOHNUNG,
  TRIUMPH,
  ROHSTOFF,
  KLINIK,
  BADEHAUS,
  ROHSTWACHS,
  SCHULE,
  ROHSTERZ,
  BERGWERK,
  PIRATWOHN,
  KIRCHE,
  MILITAR,
};

enum class WareType
{
  UNSET = 0,
  MEHL,
  GETREIDE,
  GOLD,
  KAKAO,
  WEINTRAUBEN,
  ALLWARE,
  TABAK,
  FISCHE,
  STOFFE,
  ALKOHOL,
  ERZE,
  ZIEGEL,
  GRAS,
  TABAKBAUM,
  EISENERZ,
  NOWARE,
  SCHMUCK,
  KLEIDUNG,
  SCHWERTER,
  HOLZ,
  BAUM,
  STEINE,
  KORN,
  KAKAOBAUM,
  EISEN,
  KANONEN,
  FLEISCH,
  GEWUERZE,
  TABAKWAREN,
  ZUCKER,
  NAHRUNG,
  WOLLE,
  BAUMWOLLE,
  WERKZEUG,
  GEWUERZBAUM,
  ZUCKERROHR,
  MUSKETEN,
};

enum class RohstoffType
{
  UNSET = 0,
  KORN,
  GETREIDE,
  GOLD,
  WEINTRAUBEN,
  ZUCKER,
  TABAK,
  FISCHE,
  STOFFE,
  GRAS,
  TABAKBAUM,
  MEHL,
  EISENERZ,
  NOWARE,
  HOLZ,
  BAUM,
  ALLWARE,
  KAKAOBAUM,
  EISEN,
  FLEISCH,
  STEINE,
  WOLLE,
  BAUMWOLLE,
  WILD,
  GEWUERZBAUM,
  ZUCKERROHR,
};

enum class MaxprodcntType
{
  UNSET = 0,
  MAXPRODCNT
};

enum class BauinfraType
{
  UNSET = 0,
  INFRA_STUFE_3F,
  INFRA_STUFE_1A,
  INFRA_STUFE_3B,
  INFRA_STUFE_3C,
  INFRA_KATHETRALE,
  INFRA_THEATER,
  INFRA_KIRCHE,
  INFRA_SCHULE,
  INFRA_STUFE_5A,
  INFRA_GALGEN,
  INFRA_SCHLOSS,
  INFRA_TRIUMPH,
  INFRA_STUFE_2E,
  INFRA_STUFE_2D,
  INFRA_STUFE_2G,
  INFRA_STUFE_2F,
  INFRA_STUFE_2A,
  INFRA_STUFE_2C,
  INFRA_STUFE_2B,
  INFRA_ARZT,
  INFRA_STUFE_4A,
  INFRA_WIRT,
  INFRA_BADE,
  INFRA_DENKMAL,
  INFRA_HOCHSCHULE
};

enum class FigurnrType
{
  UNSET = 0,
  TRAEGER2,
  HOLZFAELLER,
  KANONTURM,
  FISCHER,
  STEINKLOPFER,
  PFLUECKER,
  ARZT,
  JAEGER,
  MAEHER,
  PIRATTURM,
  PFLUECKER2,
  FLEISCHER,
  KARREN,
  LOESCH,
  RIND,
  TRAEGER,
  SCHAF,
  PACKESEL,
  SPEER1
};

enum class RauchfignrType
{
  UNSET = 0,
  FAHNETURM3,
  RAUCHWERKZEUG,
  RAUCHKANONEN,
  RAUCHGOLD,
  FAHNEKONTOR,
  RAUCHBAECK,
  FAHNEKONTOR3,
  FAHNEKONTOR2,
  FAHNEKONTOR1,
  RAUCHSCHWERT,
  RAUCHSCHMELZ,
  FAHNEKONTOR4,
  FAHNEMARKT,
  RAUCHBRENNER,
  FAHNETURM2,
  RAUCHMUSKET,
  FAHNETURM1
};

struct Haus
{
  int Id = -1;
  int Gfx = -1;
  int Blocknr = -1;
  int Posoffs = -1;
  int Highflg = -1;
  int Einhoffs = -1;
  int Maxenergy = -1;
  int Maxbrand = -1;
  int Rotate = -1;
  int RandAnz = -1;
  int AnimAnz = 0;
  int AnimTime = -1;
  int AnimFrame = 0;
  int AnimAdd = 0;
  int Baugfx = -1;
  int KreuzBase = -1;
  int Randwachs = -1;
  int RandAdd = -1;
  int Strandoff = -1;
  int PlaceFlg = -1;
  int NoShotFlg = -1;
  int Strandflg = -1;
  int Ausbauflg = -1;
  int Tuerflg = -1;
  int Destroyflg = -1;
  ObjectKindType Kind = ObjectKindType::UNSET;
  BausampleType Bausample = BausampleType::UNSET;
  RuinenrType Ruinenr = RuinenrType::UNSET;
  std::vector<int> Wegspeed = {};
  struct
  {
    int w;
    int h;
  } Size;
  struct
  {
    int BGruppe = -1;
    int Anicontflg = -1;
    int MakLagFlg = -1;
    int Nativflg = -1;
    int LagAniFlg = -1;
    int Doerrflg = -1;
    int NoMoreWork = -1;
    int Workmenge = -1;
    int NoLagVoll = -1;
    int Radius = -1;
    int Rohmenge = -1;
    int Prodmenge = -1;
    int Randwachs = -1;
    int Maxlager = -1;
    int Maxnorohst = -1;
    int Arbeiter = -1;
    int Figuranz = -1;
    int Interval = -1;
    WorkstoffType Workstoff = WorkstoffType::UNSET;
    ErzbergnrType Erzbergnr = ErzbergnrType::UNSET;
    ProdtypKindType Kind = ProdtypKindType::UNSET;
    WareType Ware = WareType::UNSET;
    RohstoffType Rohstoff = RohstoffType::UNSET;
    MaxprodcntType Maxprodcnt = MaxprodcntType::UNSET;
    BauinfraType Bauinfra = BauinfraType::UNSET;
    FigurnrType Figurnr = FigurnrType::UNSET;
    RauchfignrType Rauchfignr = RauchfignrType::UNSET;
    std::vector<int> Maxware = {};
    std::vector<int> Kosten = {};
  } HAUS_PRODTYP;
  struct
  {
    int Money = -1;
    int Werkzeug = -1;
    int Holz = -1;
    int Ziegel = -1;
    int Kanon = -1;
  } HAUS_BAUKOST;
};

class Haeuser
{
public:
  Haeuser(std::shared_ptr<Cod_Parser> cod)
    : cod(cod)
  {
    generate_haeuser();
  }

  std::experimental::optional<Haus*> get_haus(int id)
  {
    if (haeuser.find(id) == haeuser.end())
    {
      return {};
    }
    else
    {
      return &haeuser[id];
    }
  }
  int get_haeuser_size() { return haeuser_vec.size(); }
  Haus* get_haeuser_by_index(int index) { return haeuser_vec[index]; }

private:
  void generate_haeuser()
  {
    for (int o = 0; o < cod->objects.object_size(); o++)
    {
      auto obj = cod->objects.object(o);
      if (obj.name() == "HAUS")
      {
        for (int i = 0; i < obj.objects_size(); i++)
        {
          auto haus = generate_haus(&obj.objects(i));
          haeuser[haus.Id] = haus;
          haeuser_vec.push_back(&haeuser[haus.Id]);
        }
      }
    }
  }

  Haus generate_haus(const cod_pb::Object* obj)
  {
    Haus h;
    if (obj->has_variables() == true)
    {
      for (int i = 0; i < obj->variables().variable_size(); i++)
      {
        auto var = obj->variables().variable(i);
        if (var.name() == "Id")
        {
          if (var.value_int() == 0)
          {
            h.Id = 0;
          }
          else
          {
            h.Id = var.value_int() - id_offset;
          }
        }
        else if (var.name() == "Gfx")
        {
          h.Gfx = var.value_int();
        }
        else if (var.name() == "Blocknr")
        {
          h.Blocknr = var.value_int();
        }
        else if (var.name() == "Kind")
        {
          h.Kind = ObjectKindMap[var.value_string()];
        }
        else if (var.name() == "Posoffs")
        {
          h.Posoffs = var.value_int();
        }
        else if (var.name() == "Wegspeed")
        {
          for (int v = 0; v < var.value_array().value_size(); v++)
          {
            h.Wegspeed.push_back(var.value_array().value(v).value_int());
          }
        }
        else if (var.name() == "Highflg")
        {
          h.Highflg = var.value_int();
        }
        else if (var.name() == "Einhoffs")
        {
          h.Einhoffs = var.value_int();
        }
        else if (var.name() == "Bausample")
        {
          h.Bausample = BausampleMap[var.value_string()];
        }
        else if (var.name() == "Ruinenr")
        {
          h.Ruinenr = RuinenrMap[var.value_string()];
        }
        else if (var.name() == "Maxenergy")
        {
          h.Maxenergy = var.value_int();
        }
        else if (var.name() == "Maxbrand")
        {
          h.Maxbrand = var.value_int();
        }
        else if (var.name() == "Size")
        {
          h.Size.w = var.value_array().value(0).value_int();
          h.Size.h = var.value_array().value(1).value_int();
        }
        else if (var.name() == "Rotate")
        {
          h.Rotate = var.value_int();
        }
        else if (var.name() == "RandAnz")
        {
          h.RandAnz = var.value_int();
        }
        else if (var.name() == "AnimTime")
        {
          if (var.value_string() == "TIMENEVER")
          {
            h.AnimTime = -1;
          }
          else
          {
            h.AnimTime = var.value_int();
          }
        }
        else if (var.name() == "AnimFrame")
        {
          h.AnimFrame = var.value_int();
        }
        else if (var.name() == "AnimAdd")
        {
          h.AnimAdd = var.value_int();
        }
        else if (var.name() == "Baugfx")
        {
          h.Baugfx = var.value_int();
        }
        else if (var.name() == "PlaceFlg")
        {
          h.PlaceFlg = var.value_int();
        }
        else if (var.name() == "AnimAnz")
        {
          h.AnimAnz = var.value_int();
        }
        else if (var.name() == "KreuzBase")
        {
          h.KreuzBase = var.value_int();
        }
        else if (var.name() == "NoShotFlg")
        {
          h.NoShotFlg = var.value_int();
        }
        else if (var.name() == "Strandflg")
        {
          h.Strandflg = var.value_int();
        }
        else if (var.name() == "Ausbauflg")
        {
          h.Ausbauflg = var.value_int();
        }
        else if (var.name() == "Tuerflg")
        {
          h.Tuerflg = var.value_int();
        }
        else if (var.name() == "Randwachs")
        {
          h.Randwachs = var.value_int();
        }
        else if (var.name() == "RandAdd")
        {
          h.RandAdd = var.value_int();
        }
        else if (var.name() == "Strandoff")
        {
          h.Strandoff = var.value_int();
        }
        else if (var.name() == "Destroyflg")
        {
          h.Destroyflg = var.value_int();
        }
      }
    }
    if (obj->objects_size() > 0)
    {
      for (int o = 0; o < obj->objects_size(); o++)
      {
        auto nested_obj = obj->objects(o);
        if (nested_obj.name() == "HAUS_PRODTYP")
        {
          for (int i = 0; i < nested_obj.variables().variable_size(); i++)
          {
            auto var = nested_obj.variables().variable(i);
            if (var.name() == "Kind")
            {
              h.HAUS_PRODTYP.Kind = ProdtypKindMap[var.value_string()];
            }
            else if (var.name() == "Ware")
            {
              h.HAUS_PRODTYP.Ware = WareMap[var.value_string()];
            }
            else if (var.name() == "Workstoff")
            {
              h.HAUS_PRODTYP.Workstoff = WorkstoffMap[var.value_string()];
            }
            else if (var.name() == "Erzbergnr")
            {
              h.HAUS_PRODTYP.Erzbergnr = ErzbergnrMap[var.value_string()];
            }
            else if (var.name() == "Rohstoff")
            {
              h.HAUS_PRODTYP.Rohstoff = RohstoffMap[var.value_string()];
            }
            else if (var.name() == "MAXPRODCNT")
            {
              h.HAUS_PRODTYP.Maxprodcnt = MaxprodcntMap[var.value_string()];
            }
            else if (var.name() == "Bauinfra")
            {
              h.HAUS_PRODTYP.Bauinfra = BauinfraMap[var.value_string()];
            }
            else if (var.name() == "Figurnr")
            {
              h.HAUS_PRODTYP.Figurnr = FigurnrMap[var.value_string()];
            }
            else if (var.name() == "Rauchfignr")
            {
              h.HAUS_PRODTYP.Rauchfignr = RauchfignrMap[var.value_string()];
            }
            else if (var.name() == "Maxware")
            {
              for (int v = 0; v < var.value_array().value_size(); v++)
              {
                h.HAUS_PRODTYP.Maxware.push_back(var.value_array().value(v).value_int());
              }
            }
            else if (var.name() == "Kosten")
            {
              for (int v = 0; v < var.value_array().value_size(); v++)
              {
                h.HAUS_PRODTYP.Kosten.push_back(var.value_array().value(v).value_int());
              }
            }
            else if (var.name() == "BGruppe")
            {
              h.HAUS_PRODTYP.BGruppe = var.value_int();
            }
            else if (var.name() == "LagAniFlg")
            {
              h.HAUS_PRODTYP.LagAniFlg = var.value_int();
            }
            else if (var.name() == "NoMoreWork")
            {
              h.HAUS_PRODTYP.NoMoreWork = var.value_int();
            }
            else if (var.name() == "Workmenge")
            {
              h.HAUS_PRODTYP.Workmenge = var.value_int();
            }
            else if (var.name() == "Doerrflg")
            {
              h.HAUS_PRODTYP.Doerrflg = var.value_int();
            }
            else if (var.name() == "Anicontflg")
            {
              h.HAUS_PRODTYP.Anicontflg = var.value_int();
            }
            else if (var.name() == "MakLagFlg")
            {
              h.HAUS_PRODTYP.MakLagFlg = var.value_int();
            }
            else if (var.name() == "Nativflg")
            {
              h.HAUS_PRODTYP.Nativflg = var.value_int();
            }
            else if (var.name() == "NoLagVoll")
            {
              h.HAUS_PRODTYP.NoLagVoll = var.value_int();
            }
            else if (var.name() == "Radius")
            {
              h.HAUS_PRODTYP.Radius = var.value_int();
            }
            else if (var.name() == "Rohmenge")
            {
              h.HAUS_PRODTYP.Rohmenge = var.value_int();
            }
            else if (var.name() == "Prodmenge")
            {
              h.HAUS_PRODTYP.Prodmenge = var.value_int();
            }
            else if (var.name() == "Randwachs")
            {
              h.HAUS_PRODTYP.Randwachs = var.value_int();
            }
            else if (var.name() == "Maxlager")
            {
              h.HAUS_PRODTYP.Maxlager = var.value_int();
            }
            else if (var.name() == "Maxnorohst")
            {
              h.HAUS_PRODTYP.Maxnorohst = var.value_int();
            }
            else if (var.name() == "Arbeiter")
            {
              h.HAUS_PRODTYP.Arbeiter = var.value_int();
            }
            else if (var.name() == "Figuranz")
            {
              h.HAUS_PRODTYP.Figuranz = var.value_int();
            }
            else if (var.name() == "Interval")
            {
              h.HAUS_PRODTYP.Interval = var.value_int();
            }
          }
        }
        else if (nested_obj.name() == "HAUS_BAUKOST")
        {
          for (int i = 0; i < nested_obj.variables().variable_size(); i++)
          {
            auto var = nested_obj.variables().variable(i);
            if (var.name() == "Money")
            {
              h.HAUS_BAUKOST.Money = var.value_int();
            }
            else if (var.name() == "Werkzeug")
            {
              h.HAUS_BAUKOST.Werkzeug = var.value_int();
            }
            else if (var.name() == "Holz")
            {
              h.HAUS_BAUKOST.Holz = var.value_int();
            }
            else if (var.name() == "Ziegel")
            {
              h.HAUS_BAUKOST.Ziegel = var.value_int();
            }
            else if (var.name() == "Kanon")
            {
              h.HAUS_BAUKOST.Kanon = var.value_int();
            }
          }
        }
      }
    }
    return h;
  }

  const int id_offset = 20000;
  std::map<int, Haus> haeuser;
  std::vector<Haus*> haeuser_vec;
  std::shared_ptr<Cod_Parser> cod;

  std::map<std::string, ObjectKindType> ObjectKindMap = {{"WALD", ObjectKindType::WALD}, {"TOR", ObjectKindType::TOR}, {"RUINE", ObjectKindType::RUINE},
      {"HQ", ObjectKindType::HQ}, {"STRANDMUND", ObjectKindType::STRANDMUND}, {"STRANDHAUS", ObjectKindType::STRANDHAUS},
      {"BRANDUNG", ObjectKindType::BRANDUNG}, {"STRANDRUINE", ObjectKindType::STRANDRUINE}, {"HANGECK", ObjectKindType::HANGECK},
      {"TURM", ObjectKindType::TURM}, {"STRANDECKI", ObjectKindType::STRANDECKI}, {"STRANDVARI", ObjectKindType::STRANDVARI},
      {"STRANDECKA", ObjectKindType::STRANDECKA}, {"MEER", ObjectKindType::MEER}, {"BODEN", ObjectKindType::BODEN}, {"HANGQUELL", ObjectKindType::HANGQUELL},
      {"MAUER", ObjectKindType::MAUER}, {"FELS", ObjectKindType::FELS}, {"MUENDUNG", ObjectKindType::MUENDUNG}, {"HANG", ObjectKindType::HANG},
      {"MINE", ObjectKindType::MINE}, {"BRANDECK", ObjectKindType::BRANDECK}, {"TURMSTRAND", ObjectKindType::TURMSTRAND}, {"PIER", ObjectKindType::PIER},
      {"STRAND", ObjectKindType::STRAND}, {"BRUECKE", ObjectKindType::BRUECKE}, {"FLUSSECK", ObjectKindType::FLUSSECK}, {"FLUSS", ObjectKindType::FLUSS},
      {"HAFEN", ObjectKindType::HAFEN}, {"STRASSE", ObjectKindType::STRASSE}, {"GEBAEUDE", ObjectKindType::GEBAEUDE},
      {"MAUERSTRAND", ObjectKindType::MAUERSTRAND}, {"PLATZ", ObjectKindType::PLATZ}};

  std::map<std::string, RuinenrType> RuinenrMap = {{"RUINE_KONTOR_N1", RuinenrType::RUINE_KONTOR_N1}, {"RUINE_KONTOR_N2", RuinenrType::RUINE_KONTOR_N2},
      {"RUINE_HOLZ", RuinenrType::RUINE_HOLZ}, {"RUINE_KONTOR_1", RuinenrType::RUINE_KONTOR_1}, {"RUINE_FELD", RuinenrType::RUINE_FELD},
      {"RUINE_MARKT", RuinenrType::RUINE_MARKT}, {"RUINE_STEIN", RuinenrType::RUINE_STEIN}, {"RUINE_MINE", RuinenrType::RUINE_MINE},
      {"RUINE_ROAD_STEIN", RuinenrType::RUINE_ROAD_STEIN}, {"RUINE_ROAD_FELD", RuinenrType::RUINE_ROAD_FELD}};

  std::map<std::string, BausampleType> BausampleMap = {{"WAV_TRIUMPH", BausampleType::WAV_TRIUMPH}, {"WAV_SCHWERTBAU", BausampleType::WAV_SCHWERTBAU},
      {"WAV_PYRAMIDE", BausampleType::WAV_PYRAMIDE}, {"WAV_KATHETRALE", BausampleType::WAV_KATHETRALE}, {"WAV_NIX", BausampleType::WAV_NIX},
      {"WAV_SCHLOSS", BausampleType::WAV_SCHLOSS}, {"WAV_KAPELLE", BausampleType::WAV_KAPELLE}, {"WAV_WOHNHAUS", BausampleType::WAV_WOHNHAUS},
      {"WAV_BRUNNEN", BausampleType::WAV_BRUNNEN}, {"WAV_KONTOR", BausampleType::WAV_KONTOR}, {"WAV_GIESSEREI", BausampleType::WAV_GIESSEREI},
      {"WAV_FLEISCHER", BausampleType::WAV_FLEISCHER}, {"WAV_SCHULE", BausampleType::WAV_SCHULE}, {"WAV_DENKMAL", BausampleType::WAV_DENKMAL},
      {"WAV_BAECKER", BausampleType::WAV_BAECKER}, {"WAV_MINE", BausampleType::WAV_MINE}, {"WAV_STANDARD", BausampleType::WAV_STANDARD},
      {"WAV_BURG", BausampleType::WAV_BURG}, {"WAV_RUMBRENNER", BausampleType::WAV_RUMBRENNER}, {"WAV_PLANTAGE", BausampleType::WAV_PLANTAGE},
      {"WAV_ARZT", BausampleType::WAV_ARZT}, {"WAV_MUEHLE", BausampleType::WAV_MUEHLE}, {"WAV_PIRATEN", BausampleType::WAV_PIRATEN},
      {"WAV_JAGDHUETTE", BausampleType::WAV_JAGDHUETTE}, {"WAV_KIRCHE", BausampleType::WAV_KIRCHE}, {"WAV_MARKT", BausampleType::WAV_MARKT},
      {"WAV_BADEHAUS", BausampleType::WAV_BADEHAUS}, {"WAV_WIRTSHAUS", BausampleType::WAV_WIRTSHAUS}, {"WAV_THEATER", BausampleType::WAV_THEATER}};

  std::map<std::string, WorkstoffType> WorkstoffMap = {{"HOLZ", WorkstoffType::HOLZ}, {"STOFFE", WorkstoffType::STOFFE}};

  std::map<std::string, ErzbergnrType> ErzbergnrMap = {{"ERZBERG_KLEIN", ErzbergnrType::ERZBERG_KLEIN}, {"ERZBERG_GROSS", ErzbergnrType::ERZBERG_GROSS}};

  std::map<std::string, ProdtypKindType> ProdtypKindMap = {{"BRUNNEN", ProdtypKindType::BRUNNEN}, {"KONTOR", ProdtypKindType::KONTOR},
      {"HOCHSCHULE", ProdtypKindType::HOCHSCHULE}, {"JAGDHAUS", ProdtypKindType::JAGDHAUS}, {"STEINBRUCH", ProdtypKindType::STEINBRUCH},
      {"MARKT", ProdtypKindType::MARKT}, {"PLANTAGE", ProdtypKindType::PLANTAGE}, {"DENKMAL", ProdtypKindType::DENKMAL},
      {"FISCHEREI", ProdtypKindType::FISCHEREI}, {"WEIDETIER", ProdtypKindType::WEIDETIER}, {"SCHLOSS", ProdtypKindType::SCHLOSS},
      {"WIRT", ProdtypKindType::WIRT}, {"HANDWERK", ProdtypKindType::HANDWERK}, {"WACHTURM", ProdtypKindType::WACHTURM}, {"KAPELLE", ProdtypKindType::KAPELLE},
      {"WERFT", ProdtypKindType::WERFT}, {"THEATER", ProdtypKindType::THEATER}, {"GALGEN", ProdtypKindType::GALGEN}, {"UNUSED", ProdtypKindType::UNUSED},
      {"pMAUER", ProdtypKindType::pMAUER}, {"WOHNUNG", ProdtypKindType::WOHNUNG}, {"TRIUMPH", ProdtypKindType::TRIUMPH},
      {"ROHSTOFF", ProdtypKindType::ROHSTOFF}, {"KLINIK", ProdtypKindType::KLINIK}, {"BADEHAUS", ProdtypKindType::BADEHAUS},
      {"ROHSTWACHS", ProdtypKindType::ROHSTWACHS}, {"SCHULE", ProdtypKindType::SCHULE}, {"ROHSTERZ", ProdtypKindType::ROHSTERZ},
      {"BERGWERK", ProdtypKindType::BERGWERK}, {"PIRATWOHN", ProdtypKindType::PIRATWOHN}, {"KIRCHE", ProdtypKindType::KIRCHE},
      {"MILITAR", ProdtypKindType::MILITAR}};

  std::map<std::string, WareType> WareMap = {{"MEHL", WareType::MEHL}, {"GETREIDE", WareType::GETREIDE}, {"GOLD", WareType::GOLD}, {"KAKAO", WareType::KAKAO},
      {"WEINTRAUBEN", WareType::WEINTRAUBEN}, {"ALLWARE", WareType::ALLWARE}, {"TABAK", WareType::TABAK}, {"FISCHE", WareType::FISCHE},
      {"STOFFE", WareType::STOFFE}, {"ALKOHOL", WareType::ALKOHOL}, {"ERZE", WareType::ERZE}, {"ZIEGEL", WareType::ZIEGEL}, {"GRAS", WareType::GRAS},
      {"TABAKBAUM", WareType::TABAKBAUM}, {"EISENERZ", WareType::EISENERZ}, {"NOWARE", WareType::NOWARE}, {"SCHMUCK", WareType::SCHMUCK},
      {"KLEIDUNG", WareType::KLEIDUNG}, {"SCHWERTER", WareType::SCHWERTER}, {"HOLZ", WareType::HOLZ}, {"BAUM", WareType::BAUM}, {"STEINE", WareType::STEINE},
      {"KORN", WareType::KORN}, {"KAKAOBAUM", WareType::KAKAOBAUM}, {"EISEN", WareType::EISEN}, {"KANONEN", WareType::KANONEN}, {"FLEISCH", WareType::FLEISCH},
      {"GEWUERZE", WareType::GEWUERZE}, {"TABAKWAREN", WareType::TABAKWAREN}, {"ZUCKER", WareType::ZUCKER}, {"NAHRUNG", WareType::NAHRUNG},
      {"WOLLE", WareType::WOLLE}, {"BAUMWOLLE", WareType::BAUMWOLLE}, {"WERKZEUG", WareType::WERKZEUG}, {"GEWUERZBAUM", WareType::GEWUERZBAUM},
      {"ZUCKERROHR", WareType::ZUCKERROHR}, {"MUSKETEN", WareType::MUSKETEN}};

  std::map<std::string, RohstoffType> RohstoffMap = {{"KORN", RohstoffType::KORN}, {"GETREIDE", RohstoffType::GETREIDE}, {"GOLD", RohstoffType::GOLD},
      {"WEINTRAUBEN", RohstoffType::WEINTRAUBEN}, {"ZUCKER", RohstoffType::ZUCKER}, {"TABAK", RohstoffType::TABAK}, {"FISCHE", RohstoffType::FISCHE},
      {"STOFFE", RohstoffType::STOFFE}, {"GRAS", RohstoffType::GRAS}, {"TABAKBAUM", RohstoffType::TABAKBAUM}, {"MEHL", RohstoffType::MEHL},
      {"EISENERZ", RohstoffType::EISENERZ}, {"NOWARE", RohstoffType::NOWARE}, {"HOLZ", RohstoffType::HOLZ}, {"BAUM", RohstoffType::BAUM},
      {"ALLWARE", RohstoffType::ALLWARE}, {"KAKAOBAUM", RohstoffType::KAKAOBAUM}, {"EISEN", RohstoffType::EISEN}, {"FLEISCH", RohstoffType::FLEISCH},
      {"STEINE", RohstoffType::STEINE}, {"WOLLE", RohstoffType::WOLLE}, {"BAUMWOLLE", RohstoffType::BAUMWOLLE}, {"WILD", RohstoffType::WILD},
      {"GEWUERZBAUM", RohstoffType::GEWUERZBAUM}, {"ZUCKERROHR", RohstoffType::ZUCKERROHR}};

  std::map<std::string, MaxprodcntType> MaxprodcntMap = {{"MAXPRODCNT", MaxprodcntType::MAXPRODCNT}};

  std::map<std::string, BauinfraType> BauinfraMap = {{"INFRA_STUFE_3F", BauinfraType::INFRA_STUFE_3F}, {"INFRA_STUFE_1A", BauinfraType::INFRA_STUFE_1A},
      {"INFRA_STUFE_3B", BauinfraType::INFRA_STUFE_3B}, {"INFRA_STUFE_3C", BauinfraType::INFRA_STUFE_3C}, {"INFRA_KATHETRALE", BauinfraType::INFRA_KATHETRALE},
      {"INFRA_THEATER", BauinfraType::INFRA_THEATER}, {"INFRA_KIRCHE", BauinfraType::INFRA_KIRCHE}, {"INFRA_SCHULE", BauinfraType::INFRA_SCHULE},
      {"INFRA_STUFE_5A", BauinfraType::INFRA_STUFE_5A}, {"INFRA_GALGEN", BauinfraType::INFRA_GALGEN}, {"INFRA_SCHLOSS", BauinfraType::INFRA_SCHLOSS},
      {"INFRA_TRIUMPH", BauinfraType::INFRA_TRIUMPH}, {"INFRA_STUFE_2E", BauinfraType::INFRA_STUFE_2E}, {"INFRA_STUFE_2D", BauinfraType::INFRA_STUFE_2D},
      {"INFRA_STUFE_2G", BauinfraType::INFRA_STUFE_2G}, {"INFRA_STUFE_2F", BauinfraType::INFRA_STUFE_2F}, {"INFRA_STUFE_2A", BauinfraType::INFRA_STUFE_2A},
      {"INFRA_STUFE_2C", BauinfraType::INFRA_STUFE_2C}, {"INFRA_STUFE_2B", BauinfraType::INFRA_STUFE_2B}, {"INFRA_ARZT", BauinfraType::INFRA_ARZT},
      {"INFRA_STUFE_4A", BauinfraType::INFRA_STUFE_4A}, {"INFRA_WIRT", BauinfraType::INFRA_WIRT}, {"INFRA_BADE", BauinfraType::INFRA_BADE},
      {"INFRA_DENKMAL", BauinfraType::INFRA_DENKMAL}, {"INFRA_HOCHSCHULE", BauinfraType::INFRA_HOCHSCHULE}};

  std::map<std::string, FigurnrType> FigurnrMap
      = {{"TRAEGER2", FigurnrType::TRAEGER2}, {"HOLZFAELLER", FigurnrType::HOLZFAELLER}, {"KANONTURM", FigurnrType::KANONTURM},
          {"FISCHER", FigurnrType::FISCHER}, {"STEINKLOPFER", FigurnrType::STEINKLOPFER}, {"PFLUECKER", FigurnrType::PFLUECKER}, {"ARZT", FigurnrType::ARZT},
          {"JAEGER", FigurnrType::JAEGER}, {"MAEHER", FigurnrType::MAEHER}, {"PIRATTURM", FigurnrType::PIRATTURM}, {"PFLUECKER2", FigurnrType::PFLUECKER2},
          {"FLEISCHER", FigurnrType::FLEISCHER}, {"KARREN", FigurnrType::KARREN}, {"LOESCH", FigurnrType::LOESCH}, {"RIND", FigurnrType::RIND},
          {"TRAEGER", FigurnrType::TRAEGER}, {"SCHAF", FigurnrType::SCHAF}, {"PACKESEL", FigurnrType::PACKESEL}, {"SPEER1", FigurnrType::SPEER1}};

  std::map<std::string, RauchfignrType> RauchfignrMap = {{"FAHNETURM3", RauchfignrType::FAHNETURM3}, {"RAUCHWERKZEUG", RauchfignrType::RAUCHWERKZEUG},
      {"RAUCHKANONEN", RauchfignrType::RAUCHKANONEN}, {"RAUCHGOLD", RauchfignrType::RAUCHGOLD}, {"FAHNEKONTOR", RauchfignrType::FAHNEKONTOR},
      {"RAUCHBAECK", RauchfignrType::RAUCHBAECK}, {"FAHNEKONTOR3", RauchfignrType::FAHNEKONTOR3}, {"FAHNEKONTOR2", RauchfignrType::FAHNEKONTOR2},
      {"FAHNEKONTOR1", RauchfignrType::FAHNEKONTOR1}, {"RAUCHSCHWERT", RauchfignrType::RAUCHSCHWERT}, {"RAUCHSCHMELZ", RauchfignrType::RAUCHSCHMELZ},
      {"FAHNEKONTOR4", RauchfignrType::FAHNEKONTOR4}, {"FAHNEMARKT", RauchfignrType::FAHNEMARKT}, {"RAUCHBRENNER", RauchfignrType::RAUCHBRENNER},
      {"FAHNETURM2", RauchfignrType::FAHNETURM2}, {"RAUCHMUSKET", RauchfignrType::RAUCHMUSKET}, {"FAHNETURM1", RauchfignrType::FAHNETURM1}};
};

#endif
