#ifndef OBJECT_ANIMATIONS_H
#define OBJECT_ANIMATIONS_H

#include "haeuser.hpp"
#include "object_animation.hpp"

#include <map>
#include <memory>
#include <vector>

class Object_Animations
{
public:
  Object_Animations(std::shared_ptr<Haeuser> haeuser)
    : haeuser(haeuser)
  {
    generate_map();
  }
  std::shared_ptr<Object_Animation> get_animation(int id) { return animations_map[id]; }

private:
  void generate_map()
  {
    for (int i = 0; i < haeuser->get_haeuser_size(); i++)
    {
      int id = haeuser->get_haeuser_by_index(i)->Id;
      animations_map[id] = std::make_shared<Object_Animation>(id, haeuser);
    }
  }

  std::map<int, std::shared_ptr<Object_Animation>> animations_map;
  std::shared_ptr<Haeuser> haeuser;
};

#endif