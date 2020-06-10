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

#include <cstring>
#include <fstream>
#include <iostream>

#include "gam/chunk.hpp"
#define CHUNK_NAME_SIZE (16u)

std::vector<std::shared_ptr<Chunk>> Chunk::ReadChunks(const std::string& path)
{
  std::vector<std::shared_ptr<Chunk>> chunks;
  std::ifstream f;
  f.open(path, std::ios_base::in | std::ios_base::binary);
  while (!f.eof())
  {
    try
    {
      chunks.push_back(std::make_shared<Chunk>(f));
    }
    catch (const std::exception& e)
    {
      std::cerr << e.what() << '\n';
    }
  }
  f.close();
  return chunks;
}

Chunk::Chunk(std::istream& f)
{
  char n[CHUNK_NAME_SIZE];
  f.read(n, CHUNK_NAME_SIZE);
  if ((strnlen(n, CHUNK_NAME_SIZE) > 0) && (strnlen(n, CHUNK_NAME_SIZE) != CHUNK_NAME_SIZE))
  {
    chunk.name = std::string(n);
  }
  else
  {
    chunk.name = "";
  }
  f.read((char*)&chunk.length, sizeof(chunk.length));
  if (chunk.length > 0)
  {
    chunk.data = new uint8_t[chunk.length];
    f.read((char*)chunk.data, chunk.length);
  }
}

Chunk::~Chunk()
{
  if (chunk.length > 0)
  {
    delete[] chunk.data;
  }
}
