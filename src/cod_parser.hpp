#ifndef COD_PARSER_HPP
#define COD_PARSER_HPP

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstring>

#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/variant.hpp>

#include "external/nlohmann/json.hpp"

using nlohmann::json;

class Cod_Parser
{
public:
  Cod_Parser(std::string cod_file_path)
    : path(cod_file_path)
  {
    decode();
    convert_to_json();
  }

private:
  std::vector<std::string> regex_match(std::string regex, std::string str)
  {
    std::vector<std::string> ret;
    boost::regex expr{regex};
    boost::smatch what;
    if (boost::regex_match(str, what, expr))
    {
      for (int i = 0; i < what.size(); i++)
      {
        ret.push_back(what[i]);
      }
    }
    return ret;
  }

  std::vector<std::string> regex_search(std::string regex, std::string str)
  {
    std::vector<std::string> ret;
    boost::regex expr{regex};
    boost::smatch what;
    if (boost::regex_search(str, what, expr))
    {
      for (int i = 0; i < what.size(); i++)
      {
        ret.push_back(what[i]);
      }
    }
    return ret;
  }

  bool convert_to_json()
  {
    json variables;
    json gfx_map;

    for (auto& line : cod_txt)
    {
      line = trim_comment_from_line(line);
      if (line.size() == 0)
      {
        continue;
      }

      if (is_substring(line, "Nahrung:") || is_substring(line, "Soldat:") || is_substring(line, "Turm:") || std::all_of(line.begin(), line.end(), isspace))
      {
        // TODO : skipped for now
        continue;
      }

      // constant assignment
      std::vector<std::string> result = regex_search("(@?)(\\w+)\\s*=\\s*((?:\\d+|\\+|\\w+)+)", line);
      if (result.size() > 0)
      {
        bool is_math = is_substring(result[3], "+");
        std::string constant = result[2];
        std::string value = result[3];
        if (begins_with(constant, "GFX"))
        {
          if (value == "0")
          {
            gfx_map[constant] = value;
          }
          else if (begins_with(value, "GFX"))
          {
            std::string current_gfx = split_by_delimiter(value, "+")[0];
            if (is_in_json(gfx_map, current_gfx) == true)
            {
              gfx_map[constant] = gfx_map[current_gfx];
            }
          }
        }
        variables[constant] = get_value(constant, value, is_math, variables);
      }
    }
    std::cout << variables.dump(2) << std::endl;
    std::cout << gfx_map.dump(2) << std::endl;
  }

  json get_value(const std::string& key, const std::string& value, bool is_math, json variables)
  {
    if (is_substring(value, ","))
    {
      std::cout << key << std::endl;
    }
    if (is_math == true)
    {
      // Searching for some characters followed by a + or - sign and some digits. Example: VALUE+20
      std::vector<std::string> result = regex_search("(\\w+)(\\+|-)(\\d+)", value);
      if (result.size() > 0)
      {
        json old_val;
        if (is_in_json(variables, result[1]) == true)
        {
          old_val = variables[result[1]];
        }
        else
        {
          old_val = 0;
        }

        if (old_val.type() == json::value_t::string)
        {
          if (old_val == "RUINE_KONTOR_1")
          {
            // TODO
            old_val = 424242;
          }
        }
        if (result[2] == "+")
        {
          if (old_val.type() == json::value_t::number_integer)
          {
            json ret;
            int o = old_val;
            ret = o + std::stoi(result[3]);
            return ret;
          }
          else if (old_val.type() == json::value_t::number_float)
          {
            json ret;
            int o = old_val;
            ret = o + std::stof(result[3]);
            return ret;
          }
          else
          {
            json ret;
            std::string o = old_val;
            ret = std::stoi(old_val.get<std::string>()) + std::stoi(result[3]);
            return ret;
          }
        }
        if (result[2] == "-")
        {
          if (old_val.type() == json::value_t::number_integer)
          {
            json ret;
            int o = old_val;
            ret[key] = o - std::stoi(result[3]);
            return ret;
          }
          else if (old_val.type() == json::value_t::number_float)
          {
            json ret;
            int o = old_val;
            ret[key] = o - std::stof(result[3]);
            return ret;
          }
          {
            json ret;
            std::string o = old_val;
            ret = std::stoi(old_val.get<std::string>()) - std::stoi(result[3]);
            return ret;
          }
        }
      }
    }

    {
      // Check if value has no preceding characters, a possible + or - sign
      // and one ore more digits -> its an int
      std::vector<std::string> result = regex_match("^\\w*[\\-+]?\\d+", value);
      if (result.size() > 0)
      {
        json ret = std::stoi(value);
        return ret;
      }
    }

    {
      // Check if value has no preceding characters, a possible + or - sign and one ore more digits
      // followed by a dot and another one or more digits -> its a float
      std::vector<std::string> result = regex_match("^\\w+[\\-+]?\\d+\\.\\d+", value);
      if (result.size() > 0)
      {
        json ret = std::stof(value);
        return ret;
      }
    }

    {
      // Check if value contains any other characters besides 0-9, + and -
      // -> it is a pure string
      std::vector<std::string> result = regex_match("([A-Za-z0-9_]+)", value);
      if (result.size() > 0)
      {
        // TODO : When is value not in variables
        if (variables.count(result[1]) > 0)
        {
          json ret;
          ret = variables[result[1]];
          return ret;
        }
        else
        {
          json ret;
          ret = value;
          return ret;
        }
      }
    }

    {
      if (is_substring(value, ","))
      {
        std::vector<std::string> values = split_by_delimiter(value, ",");
        json jvalues;
        for (int i = 0; i < values.size(); i++)
        {
          jvalues[i] = get_value(key, values[i], false, variables);
        }

        if (key == "Size")
        {
          json ret;
          ret["x"] = jvalues[0];
          ret["y"] = jvalues[1];
          return ret;
        }
        else if (key == "Ware")
        {
          json ret;
          ret = jvalues[1];
          return ret;
        }
        json ret;
        ret = jvalues;
        return ret;
      }
    }
    return 0;
  }

  bool is_in_json(const json& j, const std::string& key)
  {
    return j.find(key) != j.end();
  }

  bool is_substring(std::string str, std::string substr)
  {
    std::size_t found = str.find(substr);
    if (found != std::string::npos)
    {
      return true;
    }
    return false;
  }

  std::vector<std::string> split_by_delimiter(std::string str, std::string delim)
  {
    std::vector<std::string> tokens;
    boost::split(tokens, str, boost::is_any_of(delim));
    return tokens;
  }

  std::string trim_comment_from_line(std::string str)
  {
    return split_by_delimiter(str, ";")[0];
  }

  bool begins_with(std::string str, std::string begin)
  {
    if (str.rfind(begin, 0) == 0)
    {
      return true;
    }
    return false;
  }

  bool decode()
  {
    std::ifstream input(path, std::ios::binary);
    std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(input), {});

    for (auto& c : buffer)
    {
      c = 256 - c;
    }
    std::string line;
    for (int i = 0; i < buffer.size() - 1; i++)
    {
      if (buffer[i + 1] != '\n' && buffer[i] != '\r')
      {
        line.append(1, buffer[i]);
      }
      else
      {
        cod_txt.push_back(line);
        line = "";
        i++; // hop over '\n'
      }
    }
    return true;
  }

  std::string path;
  std::vector<std::string> cod_txt;
};
#endif