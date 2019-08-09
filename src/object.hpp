#ifndef OBJECTS_HPP
#define OBJECTS_HPP

#include "object_animation.hpp"

#include <memory>

class Object
{
public:
  Object(int x, int y, std::shared_ptr<Object_Animation> animation)
    : x(x)
    , y(y)
    , animation(animation)
  {
  }

  void trigger_animation() { animation_trigger = true; }
  void animate()
  {
      
  }

private:
  bool animation_trigger;
  int x;
  int y;
  std::shared_ptr<Object_Animation> animation;
}

#endif