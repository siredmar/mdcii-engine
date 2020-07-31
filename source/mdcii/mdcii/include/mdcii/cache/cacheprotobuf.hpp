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

#ifndef _CACHE_PROTOBUF_HPP_
#define _CACHE_PROTOBUF_HPP_

#include <fstream>
#include <iostream>
#include <string>
#include <unistd.h>

#include <filesystem>

#include <google/protobuf/util/json_util.h>

#include "sago/platform_folders.h"

namespace fs = std::filesystem;

template <typename T>
class CacheProtobuf
{
public:
    explicit CacheProtobuf(const std::string& file, bool clearOnCreate = false)
        : dataPath(sago::getDataHome())
        , cacheFilePath(fs::path(dataPath.string() + "/" + file))
        , cacheFileDirectoryPath(cacheFilePath.parent_path())
    {
        if (clearOnCreate)
        {
            if (DirectoryExists())
            {
                DeleteDirectory();
            }
        }

        if (not CreateCacheFileDirectory())
        {
            throw("[EER] Cannot create cache file directroy: " + cacheFileDirectoryPath.string());
        }
    }

    void Serialize(const T& data)
    {
        std::string jsonString;
        google::protobuf::util::JsonPrintOptions options;
        options.add_whitespace = true;
        options.always_print_primitive_fields = true;
        MessageToJsonString(data, &jsonString, options);
        std::ofstream out(cacheFilePath.string());
        if (!out.is_open())
        {
            throw("[EER] Failed to open file \"" + cacheFilePath.string() + "\"");
        }
        out << jsonString;
        out.close();
    }

    T Deserialize()
    {
        T data;
        std::string jsonString;
        std::ifstream t(cacheFilePath.string());
        std::stringstream buffer;
        if (!t.is_open())
        {
            throw("[EER] Failed to open file \"" + cacheFilePath.string() + "\"");
        }
        buffer << t.rdbuf();
        jsonString = buffer.str();
        t.close();

        google::protobuf::util::JsonParseOptions options;
        options.ignore_unknown_fields = false;
        JsonStringToMessage(jsonString, &data, options);
        return data;
    }

    bool Exists()
    {
        return fs::exists(cacheFilePath);
    }

private:
    fs::path dataPath;
    fs::path cacheFilePath;
    fs::path cacheFileDirectoryPath;

    bool DeleteDirectory()
    {
        return fs::remove_all(cacheFileDirectoryPath);
    }

    bool DirectoryExists()
    {
        return fs::exists(cacheFileDirectoryPath);
    }

    bool CreateCacheFileDirectory()
    {
        if (not DirectoryExists())
        {
            bool ret;
            try
            {
                ret = fs::create_directories(cacheFileDirectoryPath);
            }
            catch (const std::exception& e)
            {
                std::cerr << e.what() << '\n';
            }

            return ret;
        }
        else
        {
            return true;
        }
    }
};

#endif // _CACHE_PROTOBUF_HPP_