#ifndef _CACHE_PROTOBUF_HPP_
#define _CACHE_PROTOBUF_HPP_

#include <fstream>
#include <iostream>
#include <string>
#include <unistd.h>

#include <experimental/filesystem>

#include <google/protobuf/util/json_util.h>

#include "sago/platform_folders.h"

namespace fs = std::experimental::filesystem;

template<typename T>
class CacheProtobuf
{
public:
  CacheProtobuf(const std::string& file)
    : dataPath(sago::getDataHome())
    , cacheFilePath(fs::path(dataPath.string() + "/" + file))
    , cacheFileDirectoryPath(cacheFilePath.parent_path())
  {
    if (not CreateCacheFileDirectory())
    {
      throw("Cannot create cache file directroy: " + cacheFileDirectoryPath.string());
    }
  }

  void Serialize(const T& data)
  {
    std::string json_string;
    google::protobuf::util::JsonPrintOptions options;
    options.add_whitespace = true;
    options.always_print_primitive_fields = true;
    MessageToJsonString(data, &json_string, options);
    std::ofstream out(cacheFilePath.string());
    out << json_string;
    out.close();
  }

  T Deserialize()
  {
    T data;
    std::string json_string;
    std::ifstream t(cacheFilePath.string());
    std::stringstream buffer;
    buffer << t.rdbuf();
    json_string = buffer.str();
    t.close();

    google::protobuf::util::JsonParseOptions options;
    options.ignore_unknown_fields = false;
    JsonStringToMessage(json_string, &data, options);
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

  bool DirecotryExists()
  {
    return fs::exists(cacheFileDirectoryPath);
  }

  bool CreateCacheFileDirectory()
  {
    if (not DirecotryExists())
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
    return false;
  }
};

#endif // _CACHE_PROTOBUF_HPP_