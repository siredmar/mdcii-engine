// This file is part of the MDCII Game Engine.
// Copyright (C) 2020  Armin Schlegel
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

#include <fstream>

#include "gam/chunk.hpp"

Chunk::Chunk(std::istream& f)
{
  int ChunkSize = 16;
  char n[ChunkSize];
  f.read(n, ChunkSize);
  chunk.name = std::string(n);
  f.read((char*)&chunk.length, sizeof(chunk.length));
  chunk.data = new uint8_t[chunk.length];
  f.read((char*)chunk.data, chunk.length);
}

Chunk::~Chunk()
{
  delete chunk.data;
}