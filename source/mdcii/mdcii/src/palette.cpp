#include "palette.hpp"

int getTransparentColor()
{
  // This value is hardcoded
  // palette index 253 = magenta: 0xff, 0x00, 0xff,
  return 253;
}

paletteRGBType getPaletteRGB(int index)
{
  paletteRGBType ret;
  ret.r = palette[index * 3];
  ret.g = palette[index * 3 + 1];
  ret.b = palette[index * 3 + 2];
  return ret;
}