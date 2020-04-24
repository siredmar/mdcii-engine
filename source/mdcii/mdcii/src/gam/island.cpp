#include <cstring>

#include "gam/island.hpp"


Island5::Island5(uint8_t* data, uint32_t length, const std::string& name)
  : name(name)
{
  memcpy((char*)&island5, data, length);
}