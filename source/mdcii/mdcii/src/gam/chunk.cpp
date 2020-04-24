#include <fstream>

#include "gam/chunk.hpp"

Chunk::Chunk(std::istream& f)
{
  f.read(chunk.name, sizeof(chunk.name));
  f.read((char*)&chunk.length, sizeof(chunk.length));
  chunk.data = new uint8_t[chunk.length];
  f.read((char*)chunk.data, chunk.length);
}

Chunk::~Chunk()
{
  delete chunk.data;
}