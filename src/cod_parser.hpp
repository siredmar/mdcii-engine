#ifndef COD_PARSER_HPP
#define COD_PARSER_HPP

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <regex>
#include <cstring>

#include "boost/algorithm/string.hpp"
#include <boost/variant.hpp>

#include "external/nlohmann/json.hpp"

#include "files.hpp"

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
  typedef boost::variant<int, float, std::string> variant_type;

  enum class Cod_value_type
  {
    Integer = 0,
    Float,
    String
  };

  std::vector<std::string> regex_test(std::string regex, std::string str)
  {
    std::string s;
    std::vector<std::string> ret;
    const std::regex r(regex);
    std::smatch sm;
    try
    {
      if (std::regex_search(str, sm, r))
      {
        for (int i = 1; i < sm.size(); i++)
        {
          ret.push_back(sm[i]);
        }
      }
    }
    catch (std::regex_error& e)
    {
      std::cout << e.what() << std::endl; // Syntax error in the regular expression
    }
    return ret;
  }

  bool convert_to_json()
  {
    std::map<std::string, variant_type> gfx_map;
    std::map<std::string, variant_type> variables;

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
      std::vector<std::string> result = regex_test("(@?)(\\w+)\\s*=\\s*((?:\\d+|\\+|\\w+)+)", line);
      if (result.size() > 0)
      {
        bool is_math = is_substring(result[2], "+");
        std::string constant = result[1];
        std::string value = result[2];
        if (begins_with(constant, "GFX"))
        {
          if (value == "0")
          {
            gfx_map[constant] = value;
          }
          else if (begins_with(value, "GFX"))
          {
            std::string current_gfx = split_by_delimiter(value, "+")[0];
            if (gfx_map.count(current_gfx) > 0)
            {
              gfx_map[constant] = gfx_map[current_gfx];
            }
          }
        }
        variables[constant] = get_value(constant, value, is_math, variables);
      }
    }
    for (auto e : variables)
    {
      std::cout << e.first << ": " << e.second << std::endl;
    }
  }

  variant_type get_value(const std::string& key, const std::string& value, bool is_math, std::map<std::string, variant_type> variables)
  {
    Cod_value_type value_type = get_cod_value_type(value);
    if (is_math == true)
    {
      // Searching for some characters followed by a + or - sign and some digits. Example: VALUE+20
      std::vector<std::string> result = regex_test("(\\w+)(\\+|-)(\\d+)", value);
      if (result.size() > 0)
      {
        variant_type old_val;
        if (variables.count(result[0]) > 0)
        {
          old_val = variables[result[0]];
        }
        else
        {
          old_val = 0;
        }
        // if (value_type == Cod_value_type::String)
        // {
        //   if (boost::get<std::string>(old_val) == "RUINE_KONTOR_1")
        //   {
        //     // TODO
        //     old_val = "424242";
        //   }
        // }
        if (result[1] == "+")
        {
          if (get_cod_value_type(old_val) == Cod_value_type::Integer)
            return boost::get<int>(old_val) + std::stoi(result[2]);
          else if (get_cod_value_type(old_val) == Cod_value_type::Float)
            return boost::get<float>(old_val) + std::stof(result[2]);
          else
            return std::stoi(boost::get<std::string>(old_val)) + std::stoi(result[2]);
        }
        if (result[1] == "-")
        {
          if (get_cod_value_type(old_val) == Cod_value_type::Integer)
            return boost::get<int>(old_val) - std::stoi(result[2]);
          else if (get_cod_value_type(old_val) == Cod_value_type::Float)
            return boost::get<float>(old_val) - std::stof(result[2]);
          else
            return std::stoi(boost::get<std::string>(old_val)) - std::stoi(result[2]);
        }
      }
    }

    {
      // Check if value contains any other characters besides 0-9, + and - -> it is a pure string
      std::vector<std::string> result = regex_test("([^0-9+-]*)", value);
      if (result.size() > 0)
      {
        // TODO : When is value not in variables
        if (variables.count(result[0]) > 0)
        {
          return variables[result[0]];
        }
        else
        {
          return value;
        }
      }
    }

    {
      // Check if value has no preceding characters, a possible + or - sign and one ore more digits -> its an int
      std::vector<std::string> result = regex_test("[^\\w+][\\-+]?(\\d+)", value);
      if (result.size() > 0)
      {
        return std::stoi(result[0]);
      }
    }

    {
      // Check if value has no preceding characters, a possible + or - sign and one ore more digits
      // followed by a dot and another one or more digits -> its a float
      std::vector<std::string> result = regex_test("[\\-+]?(\\d+)\\.(\\d+)", value);
      if (result.size() > 0)
      {
        return std::stof(result[0]);
      }
    }

    return 0;
  }

  Cod_value_type get_cod_value_type(variant_type value)
  {
    if (std::string(value.type().name()) == "i")
      return Cod_value_type::Integer;
    else if (std::string(value.type().name()) == "f")
      return Cod_value_type::Float;
    else
      return Cod_value_type::String;
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
    auto files = Files::instance();
    if (files->instance()->check_file(path) == false)
    {
      std::cout << "[ERR] could not open cod file: " << path << std::endl;
      return false;
    }
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