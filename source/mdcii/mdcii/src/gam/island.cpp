#include <cstring>

#include "gam/island.hpp"


Island5::Island5(uint8_t* data, uint32_t length)
{
  memcpy((char*)&island5, data, length);
}