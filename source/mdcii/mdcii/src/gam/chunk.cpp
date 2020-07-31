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

std::vector<char> Chunk::ReadFile(const std::string& filename)
{
    std::ifstream ifs(filename, std::ios::binary | std::ios::ate);
    std::ifstream::pos_type pos = ifs.tellg();

    std::vector<char> result(pos);
    if (!ifs.is_open())
    {
        throw("[EER] Failed to open file \"" + filename + "\"");
    }
    ifs.seekg(0, std::ios::beg);
    ifs.read(&result[0], pos);
    ifs.close();
    return result;
}

std::vector<std::shared_ptr<Chunk>> Chunk::ReadChunks(const std::string& path)
{
    std::vector<std::shared_ptr<Chunk>> chunks;
    std::vector<char> bufferVector = Chunk::ReadFile(path);
    uint32_t bufferLength = bufferVector.size();
    char* buffer = &bufferVector[0];
    uint32_t chunksLength = 0;
    while (chunksLength < bufferLength)
    {
        auto c = std::make_shared<Chunk>(buffer);
        auto currentChunkLength = c->GetLength();
        chunksLength += currentChunkLength;
        buffer += currentChunkLength;
        chunks.push_back(c);
    }
    return chunks;
}

Chunk::Chunk(const char* buffer)
{
    char n[CHUNK_NAME_SIZE];
    memcpy(&n, &buffer[0], CHUNK_NAME_SIZE);
    if ((strnlen(n, CHUNK_NAME_SIZE) > 0) && (strnlen(n, CHUNK_NAME_SIZE) != CHUNK_NAME_SIZE))
    {
        chunk.name = std::string(n);
    }
    else
    {
        chunk.name = "";
    }
    memcpy(&chunk.length, &buffer[CHUNK_NAME_SIZE], sizeof(chunk.length));
    if (chunk.length > 0)
    {
        chunk.data = new uint8_t[chunk.length];
        memcpy(chunk.data, &buffer[CHUNK_NAME_SIZE + sizeof(chunk.length)], chunk.length);
    }
    chunkLength = CHUNK_NAME_SIZE + sizeof(chunk.length) + chunk.length;
}

uint32_t Chunk::GetLength()
{
    return chunkLength;
}

Chunk::~Chunk()
{
    if (chunk.length > 0)
    {
        delete[] chunk.data;
    }
}
