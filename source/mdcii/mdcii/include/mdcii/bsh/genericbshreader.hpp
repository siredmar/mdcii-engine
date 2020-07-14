
// This file is part of the MDCII Game Engine.
// Copyright (C) 2020  Armin Schlegel
// Copyright (C) 2015  Benedikt Freisen
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#ifndef _BSHREADERINTERFACE_HPP
#define _BSHREADERINTERFACE_HPP

#include <boost/iostreams/device/mapped_file.hpp>
#include <stdexcept>

#include "../block.hpp"

template<int N>
struct GenericBshImage
{
  unsigned int width;
  unsigned int height;
  unsigned int typ;
  unsigned int length;
  unsigned char buffer[1];

  enum
  {
    extraColums = N
  };
};

template<class IMAGE_T>
class GenericBshReader
{
  boost::iostreams::mapped_file_source bsh;
  uint32_t imageCount;
  uint32_t* imageIndex;

  bool ImageHasNoErrors(uint32_t index)
  {
    IMAGE_T& image = GetBshImage(index);

    if (image.width == 0 || image.height == 0)
      return false;

    uint32_t width = image.width + IMAGE_T::extraColums;

    unsigned int i = 0;
    unsigned int x = 0;
    unsigned int y = 0;

    while (i < image.length - 16)
    {
      uint8_t ch = image.buffer[i++];

      if (ch == 0xff)
      {
        return true;
      }
      else if (ch == 0xfe)
      {
        x = 0;
        y++;
        if (y == image.height)
          return false;
      }
      else
      {
        x += ch;
        if (x > width)
          return false;

        if (i >= image.length - 16)
          return false;
        ch = image.buffer[i++];

        x += ch;
        if (x > width)
          return false;
        i += ch;
        if (i > image.length - 16)
          return false;
      }
    }
    return false;
  }

protected:
  explicit GenericBshReader(const std::string& path, std::string signatur)
    : bsh(boost::iostreams::mapped_file_source(path))
  {
    // Ist die Datei groß genug für einen BSH-Header?
    if (bsh.size() < 20)
      throw std::range_error("[Err] bsh: header too small");

    auto bshHeader = reinterpret_cast<const Block*>(bsh.data());

    // Trägt der Header die Kennung "BSH"?
    if (strcmp(bshHeader->kennung, signatur.substr(0, 15).c_str()) != 0)
      throw std::invalid_argument("[Err] bsh: not a bsh file");

    // Ist die Datei groß genug?
    if (bsh.size() < bshHeader->length + 20)
      throw std::range_error("[Err] bsh: structure exceeds end of file");

    // Enthält die Datei überhaupt Bilder?
    if (bshHeader->length < 4)
    {
      imageCount = 0;
      return;
    }

    imageIndex = (uint32_t*)(bsh.data() + 20);
    imageCount = imageIndex[0] / 4;

    // prüfe, ob der Indexblock groß genug ist
    if (bsh.size() < imageCount * 4 + 20)
      throw std::range_error("[Err] bsh: index block exceeds end of file");

    // Der erste Indexeintrag gibt indirekt die Größe des Index an.
    for (uint32_t i = 1; i < imageCount; i++)
    {
      if (imageIndex[i] < imageIndex[0])
        throw std::logic_error("[Err] bsh: first index entry does not have smallest offset");
    }

    // Liegen alle Bilder innerhalb der Datei?
    const char* ptr = bsh.data();
    for (uint32_t i = 0; i < imageCount; i++)
    {
      uint32_t offs = imageIndex[i] + 20;

      if (((offs + 16) > bsh.size()) || (offs + reinterpret_cast<const IMAGE_T*>(ptr + offs)->length > bsh.size()))
        throw std::range_error("[Err] bsh: picture exceeds end of file");
    }

    // prüfe Integrität der einzelnen Bilder
    for (uint32_t i = 0; i < imageCount; i++)
    {
      if (!ImageHasNoErrors(i))
        throw std::logic_error("[Err] bsh: picture bytestream contains errors");
    }
  }

public:
  IMAGE_T& GetBshImage(uint32_t index)
  {
    if (index >= imageCount)
      throw std::range_error("[Err] bsh: index out of range");
    const char* ptr = bsh.data();
    uint32_t offs = imageIndex[index] + 20;
    return *(const_cast<IMAGE_T*>(reinterpret_cast<const IMAGE_T*>(ptr + offs)));
  }

  uint32_t Count()
  {
    return imageCount;
  }
};

#endif // _BSHREADERINTERFACE_HPP
