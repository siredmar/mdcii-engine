
#include <cstring>
#include <ios>
#include <iostream>
#include <memory>
#include <variant>

#include "gam/kontor.hpp"


Kontor2::Kontor2(uint8_t* data, uint32_t length, const std::string& name)
  : name(name)
{
  Kontor2Data m;
  int numKontors = length / sizeof(Kontor2Data);
  for (int i = 0; i < numKontors; i++)
  {
    int kontorLength = length / numKontors;
    memset((char*)&m, 0, kontorLength);
    memcpy((char*)&m, data + (i * kontorLength), kontorLength);
    kontors.push_back(m);
  }
}