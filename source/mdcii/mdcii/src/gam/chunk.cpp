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