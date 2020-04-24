#include <cstring>

#include "gam/mission.hpp"


Mission::Mission(uint8_t* data, uint32_t length, const std::string& name)
  : name(name)
{
  memcpy((char*)&mission, data, length);
}