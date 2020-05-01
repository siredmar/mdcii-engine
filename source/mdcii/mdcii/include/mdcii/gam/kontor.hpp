#ifndef _KONTOR_HPP_
#define _KONTOR_HPP_

#include <inttypes.h>
#include <string>
#include <vector>

struct KontorWare
{
  uint32_t vkpreis : 10;
  uint32_t ekpreis : 10;
  uint32_t vkflg : 1;
  uint32_t ekflg : 1;
  uint32_t lagerres : 16;
  uint32_t ownlager : 16;
  uint32_t minlager : 16;
  uint32_t bedarf : 16;
  uint32_t lager;
  uint32_t hausid : 16;
};

#define MAXKONTWARESAVE 50
struct Kontor2Data
{
  uint32_t inselnr : 8;
  uint32_t posx : 8;
  uint32_t posy : 8;
  uint32_t stadtnr : 4;
  KontorWare waren[50]; //  ACHTUNG falls WARE_MAX > 50
};

class Kontor2
{
public:
  Kontor2(uint8_t* data, uint32_t length, const std::string& name);
  std::vector<Kontor2Data> kontors;

private:
  std::string name;
};


#endif // _KONTOR_HPP_