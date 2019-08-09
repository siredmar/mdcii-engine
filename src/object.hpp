#ifndef OBJECTS_HPP
#define OBJECTS_HPP

#include "object_animation.hpp"
#include "object_animations.hpp"

#include <memory>
#include <tuple>
#include <vector>

class Object
{
public:
  Object(int id, int x, int y, int rot, std::shared_ptr<Object_Animations> animations)
    : x(x)
    , y(y)
    , animations(animations)
    , animation_trigger(false)
    , animimation_steps(0)
    , current_animation_step(0)
    , rot(rot)
  {
    ani = animations->get_animation(id);
    animimation_steps = ani->animation->get_animation(rot).size();
    size = ani->animation->get_size();
  }

  void trigger_animation()
  {
    animation_trigger = true;
  }

  void animate()
  {
    // TODO: Add timing from SDL
    if (++current_animation_step % animimation_steps == 0)
    {
      current_animation_step = 0;
    }
  }

  std::vector<std::tuple<int, int, int>> draw()
  {
    std::vector<std::tuple<int, int, int>> ret;
    int x_offset = 0;
    int y_offset = 0;
    for (int i = 0 + rot; i < size.height * size.width + rot; i++)
    {
      int gfx = ani->animation->get_animation_tile(rot, current_animation_step, i % (size.height * size.width)).gfx;
      std::tuple<int, int, int> coordinates = {x + x_offset, y + y_offset, gfx};
      x_offset++;
      ret.push_back(coordinates);
      if (i - rot == size.width - 1)
      {
        y_offset++;
        x_offset = 0;
      }
    }
    animate();
    return ret;
  }

private:
  bool animation_trigger;
  int animimation_steps;
  int current_animation_step;
  int x;
  int y;
  int rot;
  Object_Animation::Size size;
  std::shared_ptr<Object_Animations::Animation> ani;
  std::shared_ptr<Object_Animations> animations;
};

#endif