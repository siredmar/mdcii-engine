#include <cstring>

#include "gam/template.hpp"


Template::Template(uint8_t* data, uint32_t length, const std::string& name)
  : name(name)
{
  memcpy((char*)&template, data, length);
}