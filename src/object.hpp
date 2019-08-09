#ifndef OBJECTS_HPP
#define OBJECTS_HPP

#include "object_animation.hpp"
#include "object_animations.hpp"

#include <memory>

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

  int animate()
  {
    // TODO: Add timing from SDL
    if (++current_animation_step % animimation_steps == 0)
    {
      current_animation_step = 0;
    }
    return current_animation_step;
  }

  void draw()
  {
    // switch (rot)
    // {
    //     // TODO: Mapping for each view
    //   case 0:
    int x = 0;
    int y = 0;
    for (int i = 0; i < size.height * size.width; i++)
    {
      int gfx = ani->animation->get_animation_tile(rot, current_animation_step, i).gfx;
      x++;
      std::cout << x << "," << y << ": " << gfx << "\t";
      if (i == size.width - 1)
      {
        y++;
        x = 0;
        std::cout << std::endl;
      }
    }
    animate();
    std::cout << std::endl;
    // break;
    //   case 1:
    //     for (int x = 0; x < size.height; x++)
    //     {
    //       for (int y = 0; y < size.width; y++)
    //       {
    //         std::cout << ani->animation->get_animation_tile(rot, current_animation_step, x * y + y).gfx << " ";
    //       }
    //       std::cout << std::endl;
    //     }
    //     break;
    //   case 2:
    //     for (int x = 0; x < size.height; x++)
    //     {
    //       for (int y = 0; y < size.width; y++)
    //       {
    //         std::cout << ani->animation->get_animation_tile(rot, current_animation_step, x * y + y).gfx << " ";
    //       }
    //       std::cout << std::endl;
    //     }
    //     break;
    //   case 3:
    //     for (int x = 0; x < size.height; x++)
    //     {
    //       for (int y = 0; y < size.width; y++)
    //       {
    //         std::cout << ani->animation->get_animation_tile(rot, current_animation_step, x * y + y).gfx << " ";
    //       }
    //       std::cout << std::endl;
    //     }
    //     break;
    // }
    //
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