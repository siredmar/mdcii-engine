#ifndef _GENERIC_HPP_
#define _GENERIC_HPP_

#include <inttypes.h>

struct FPos
{
  uint8_t a; // TODO: field position? bit field? what is that?
};

struct IPos
{
  uint8_t x; // TODO: coordinate on island with max 255? guess?
  uint8_t y; // TODO: coordinate on island with max 255? guess?
};

struct IsoObj
{
  uint8_t kind;
  union {
    uint8_t inselnr;
    FPos posh;
  };
  union {
    IPos pos;
    uint16_t nr;
    struct
    {
      uint8_t stadtnr;
      uint8_t leer1;
    };
  };
};

#endif // _GENERIC_HPP_