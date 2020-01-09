#ifndef _BASEGAD_DAT_H_
#define _BASEGAD_DAT_H_

#include <experimental/optional>
#include <map>

#include "../cod_parser.hpp"


enum class KindType
{
  UNSET = 0,
  GAD_GDX
};

struct Gadget
{
  typedef struct GadgetPos
  {
    int x;
    int y;
  };

  typedef struct GadgetSize
  {
    int h;
    int w;
  };

  int Id = -1;
  int Blocknr = -1;
  int Gfxnr = -1;
  KindType Kind = KindType::UNSET;
  int Noselflg = -1;
  int Pressoff = -1;
  GadgetPos Pos = {-1, -1};
  GadgetSize Size = {-1, -1};
};

class Basegad
{
public:
  Basegad(std::shared_ptr<Cod_Parser> cod)
    : cod(cod)
  {
    generate_gadgets();
  }

  std::experimental::optional<Gadget*> get_gadget(int id)
  {
    if (gadgets.find(id) == gadgets.end())
    {
      return {};
    }
    else
    {
      return &gadgets[id];
    }
  }
  int get_gadgets_size() { return gadgets_vec.size(); }
  Gadget* get_gadgets_by_index(int index) { return gadgets_vec[index]; }

private:
  void generate_gadgets()
  {
    for (int o = 0; o < cod->objects.object_size(); o++)
    {
      auto obj = cod->objects.object(o);
      if (obj.name() == "GADGET")
      {
        for (int i = 0; i < obj.objects_size(); i++)
        {
          auto gadget = generate_gadget(&obj.objects(i));
          gadgets[gadget.Id] = gadget;
          gadgets_vec.push_back(&gadgets[gadget.Id]);
        }
      }
    }
  }

  Gadget generate_gadget(const cod_pb::Object* obj)
  {
    Gadget h;
    if (obj->has_variables() == true)
    {
      for (int i = 0; i < obj->variables().variable_size(); i++)
      {
        if (obj->has_variables() == true)
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
          else if (var.name() == "Blocknr")
          {
            h.Blocknr = var.value_int();
          }
          else if (var.name() == "Gfxnr")
          {
            h.Gfxnr = var.value_int();
          }
          else if (var.name() == "Kind")
          {
            h.Kind = KindMap[var.value_string()];
          }
          else if (var.name() == "Noselflg")
          {
            h.Noselflg = var.value_int();
          }
          else if (var.name() == "Pressoff")
          {
            h.Pressoff = var.value_int();
          }
          else if (var.name() == "Blocknr")
          {
            h.Blocknr = var.value_int();
          }
          else if (var.name() == "Pos")
          {
            h.Pos.x = var.value_array().value(0).value_int();
            h.Pos.y = var.value_array().value(1).value_int();
          }
          else if (var.name() == "Size")
          {
            h.Size.w = var.value_array().value(0).value_int();
            h.Size.h = var.value_array().value(1).value_int();
          }
        }
      }
      return h;
    }
  }

private:
  const int id_offset = 30000;
  std::map<int, Gadget> gadgets;
  std::vector<Gadget*> gadgets_vec;
  std::shared_ptr<Cod_Parser> cod;

  std::map<std::string, KindType> KindMap = {{"GAD_GFX", KindType::GAD_GDX}};
};

#endif //_BASEGAD_DAT_H_