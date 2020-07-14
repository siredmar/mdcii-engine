
// This file is part of the MDCII Game Engine.
// Copyright (C) 2020  Armin Schlegel
// Copyright (C) 2017  Benedikt Freisen
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

#ifndef _BSHWRITER_HPP_
#define _BSHWRITER_HPP_

#include <cstdint>
#include <fstream>
#include <vector>

class BshWriter
{
public:
  explicit BshWriter(int transparentColor = 0, int extraColumns = 0, bool isZei = false);
  ~BshWriter();
  void WriteBsh(uint8_t* image, int width, int height, std::vector<uint8_t>& target);
  void ReadPGM(const char* path, uint8_t*& image, int& width, int& height);
  void AttachPGM(const char* path);
  void WriteFile(const char* path);

private:
  int transparentColor;
  int extraColumns;
  bool isZei;

  struct ImageWithMeta
  {
    uint32_t width;
    uint32_t height;
    uint32_t typ;
    uint32_t length;
    uint32_t crc;
    uint32_t offset;
    int duplicateOf;
    std::vector<uint8_t> data;
  };
  std::vector<ImageWithMeta> images;
};

#endif // _BSHWRITER_HPP_
