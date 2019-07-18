#ifndef COD_PARSER_HPP
#define COD_PARSER_HPP

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstring>
#include <stack>
#include <regex>

#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/variant.hpp>

#include <google/protobuf/util/json_util.h>
#include "proto/cod.pb.h"

class Cod_Parser
{
public:
  Cod_Parser(std::string cod_file_path)
    : path(cod_file_path)
  {
    decode();
    convert_to_json();
    // json();
  }

private:
  cod_pb::Objects objects;
  cod_pb::Object* current_object;
  struct ObjectType
  {
    cod_pb::Object* object;
    bool number_object = {false};
  };
  std::stack<ObjectType> object_stack;

  bool json()
  {
    std::string json_string;
    google::protobuf::util::JsonPrintOptions options;
    options.add_whitespace = true;
    options.always_print_primitive_fields = true;
    MessageToJsonString(objects, &json_string, options);
    std::cout << json_string << std::endl;
  }

  bool Top_is_number_object()
  {
    if (!object_stack.empty())
    {
      return object_stack.top().number_object;
    }
    return false;
  }

  cod_pb::Object* Create_object(bool number_object)
  {
    cod_pb::Object* ret;
    if (object_stack.empty())
    {
      ret = objects.add_object();
    }
    else
    {
      ret = object_stack.top().object->add_objects();
    }
    ObjectType obj;
    obj.object = ret;
    obj.number_object = number_object;
    object_stack.push(obj);
    return ret;
  }

  bool Object_finished()
  {
    if (object_stack.size() > 0)
    {
      object_stack.pop();
    }
  }

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
    cod_pb::Variables variables;
    cod_pb::Variables object_variables;
    cod_pb::Map gfx_map;

    for (auto& line : cod_txt)
    {
      line = trim_comment_from_line(line);
      if (is_substring(line, "Nahrung:") || is_substring(line, "Soldat:") || is_substring(line, "Turm:")) // || std::all_of(line.begin(), line.end(), isspace))
      {
        // TODO : skipped for now
        continue;
      }

      {
        // constant assignment
        std::vector<std::string> result = regex_search("(@?)(\\w+)\\s*=\\s*((?:\\d+|\\+|\\w+)+)", line);
        if (result.size() > 0)
        {
          bool is_math = is_substring(result[3], "+");
          std::string constant = result[2];
          std::string value = result[3];
          // if (begins_with(constant, "GFX"))
          // {
          //   if (value == "0")
          //   {
          //     gfx_map[constant] = value;
          //   }
          //   else if (begins_with(value, "GFX"))
          //   {
          //     std::string current_gfx = split_by_delimiter(value, "+")[0];
          //     if (is_in_json(gfx_map, current_gfx) == true)
          //     {
          //       gfx_map[constant] = gfx_map[current_gfx];
          //     }
          //   }
          // }add_variableadd_variabadd_variable
          int i = exists(variables, constant);
          cod_pb::Variable* variable;
          if (i != -1)
          {
            auto var = variables.mutable_variable(i);
            *var = get_value(constant, value, is_math, variables);
          }
          else
          {
            auto variable = variables.add_variable();
            *variable = get_value(constant, value, is_math, variables);
          }
          continue;
        }
      }
      {
        if (is_substring(line, ",") == true)
        {
          std::vector<std::string> result = regex_search("(\\b(?!Objekt|ObjFill\\b)\\w+)\\s*:\\s*(.+)", line);
          if (result.size() > 0)
          {
            std::vector<std::string> values = split_by_delimiter(result[2], ",");
            for (auto& v : values)
            {
              v = trim_spaces_leading_trailing(v);
            }
            auto var = current_object->mutable_variables()->add_variable();
            var->set_name(result[1]);
            auto arr = var->mutable_value_array();
            for (auto v : values)
            {
              if (check_type(v) == Cod_value_type::INT)
              {
                arr->add_value()->set_value_int(std::stoi(v));
              }
              else if (check_type(v) == Cod_value_type::FLOAT)
              {
                arr->add_value()->set_value_float(std::stof(v));
              }
              else
              {
                arr->add_value()->set_value_string(v);
              }
            }
          }
          continue;
        }
      }
      {
        std::vector<std::string> result = regex_search("(\\b(?!Objekt|Nummer\\b)\\w+)\\s*:\\s*([+|-]?)(\\d+)", line);
        if (result.size() > 0)
        {
          auto var = current_object->mutable_variables()->add_variable();
          var->set_name(result[1]);
          var->set_value_int(std::stoi(result[3]));
          continue;
        }
      }
      {
        std::vector<std::string> result = regex_search("(\\b(?!Objekt|Nummer\\b)\\w+)\\s*:\\s*(\\w+)", line);
        if (result.size() > 0)
        {
          auto var = current_object->mutable_variables()->add_variable();
          var->set_name(result[1]);
          var->set_value_string(result[2]);
          continue;
        }
      }
      {
        std::vector<std::string> result = regex_search("Objekt:\\s*([\\w,]+)", line);
        if (result.size() > 0)
        {
          current_object = Create_object(false);
          current_object->set_name(result[1]);
          continue;
        }
      }
      {
        // TODO: do the calculation for name
        std::vector<std::string> result = regex_search("Nummer:\\s*([+|-]?\\d+)", line);
        if (result.size() > 0)
        {
          current_object = Create_object(true);
          current_object->set_name(result[1]);
          continue;
        }
      }
      {
        // TODO: get the constants value
        std::vector<std::string> result = regex_search("Nummer:\\s*(\\w+)", line);
        if (result.size() > 0)
        {
          current_object = Create_object(true);
          current_object->set_name(result[1]);
          continue;
        }
      }
      {
        if (is_empty(line))
        {
          if (Top_is_number_object() == true)
          {
            Object_finished();
          }
          continue;
        }
      }
      {
        std::vector<std::string> result = regex_search("\\s*EndObj", line);
        if (result.size() > 0)
        {
          Object_finished();
          continue;
        }
      }
    }
    // std::cout << variables.DebugString() << std::endl;
    std::cout << objects.DebugString() << std::endl;
  }

  cod_pb::Variable get_value(const std::string& key, const std::string& value, bool is_math, cod_pb::Variables variables)
  {
    cod_pb::Variable ret;
    ret.set_name(key);
    if (is_math == true)
    {
      // Searching for some characters followed by a + or - sign and some digits. Example: VALUE+20
      std::vector<std::string> result = regex_search("(\\w+)(\\+|-)(\\d+)", value);
      if (result.size() > 0)
      {
        cod_pb::Variable old_val;
        int i = exists(variables, result[1]);
        if (i != -1)
        {
          old_val = variables.variable(i); // get_value(variables, result[1]);
        }
        else
        {
          old_val.set_value_int(0);
        }

        if (old_val.Value_case() == cod_pb::Variable::ValueCase::kValueString)
        {
          if (old_val.value_string() == "RUINE_KONTOR_1")
          {
            // TODO
            old_val.set_value_int(424242);
          }
        }
        if (result[2] == "+")
        {
          if (old_val.Value_case() == cod_pb::Variable::ValueCase::kValueInt)
          {
            ret.set_value_int(old_val.value_int() + std::stoi(result[3]));
            return ret;
          }
          else if (old_val.Value_case() == cod_pb::Variable::ValueCase::kValueFloat)
          {
            ret.set_value_int(old_val.value_float() + std::stof(result[3]));
            return ret;
          }
          else
          {
            std::string o = old_val.value_string();
            ret.set_value_int(std::stoi(o) + std::stoi(result[3]));
            return ret;
          }
        }

        if (result[2] == "-")
        {
          if (old_val.Value_case() == cod_pb::Variable::ValueCase::kValueInt)
          {
            ret.set_value_int(old_val.value_int() - std::stoi(result[3]));
            return ret;
          }
          else if (old_val.Value_case() == cod_pb::Variable::ValueCase::kValueFloat)
          {
            ret.set_value_int(old_val.value_float() - std::stof(result[3]));
            return ret;
          }
          else
          {
            ret.set_value_int(std::stoi(old_val.value_string()) - std::stoi(result[3]));
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
        ret.set_value_int(std::stoi(value));
        return ret;
      }
    }

    {
      // Check if value has no preceding characters, a possible + or - sign and one ore more digits
      // followed by a dot and another one or more digits -> its a float
      std::vector<std::string> result = regex_match("^\\w+[\\-+]?\\d+\\.\\d+", value);
      if (result.size() > 0)
      {
        ret.set_value_int(std::stof(value));
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
        if (exists(variables, key) == true)
        {
          auto v = get_value(variables, result[1]);
          ret = v;
          ret.set_name(key);
          return ret;
        }
        else
        {
          ret.set_value_string(value);
          return ret;
        }
      }
    }

    // {
    //   if (is_substring(value, ","))
    //   {
    //     std::vector<std::string> values = split_by_delimiter(value, ",");
    //     json jvalues;
    //     for (int i = 0; i < values.size(); i++)
    //     {
    //       jvalues[i] = get_value(key, values[i], false, variables);
    //     }

    //     if (key == "Size")
    //     {
    //       json ret;
    //       ret["x"] = jvalues[0];
    //       ret["y"] = jvalues[1];
    //       return ret;
    //     }
    //     else if (key == "Ware")
    //     {
    //       json ret;
    //       ret = jvalues[1];
    //       return ret;
    //     }
    //     json ret;
    //     ret = jvalues;
    //     return ret;
    //   }
    // }
    ret.set_value_int(0);
    return ret;
  }

  int exists(const cod_pb::Variables& v, const std::string& key)
  {
    for (int i = 0; i < v.variable_size(); i++)
    {
      if (v.variable(i).name() == key)
      {
        return i;
      }
    }
    return -1;
  }

  cod_pb::Variable get_value(const cod_pb::Variables& v, const std::string& key)
  {
    for (int i = 0; i < v.variable_size(); i++)
    {
      if (v.variable(i).name() == key)
      {
        return v.variable(i);
      }
    }
    cod_pb::Variable ret;
    return ret;
  }

  std::string trim_spaces_leading_trailing(const std::string& s)
  {
    std::string input = s;
    boost::algorithm::trim(input);
    return input;
  }

  bool is_empty(const std::string& str)
  {
    if (str.size() == 0 || std::all_of(str.begin(), str.end(), isspace))
    {
      return true;
    }
    return false;
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

  enum class Cod_value_type
  {
    INT = 0,
    FLOAT,
    STRING
  };

  Cod_value_type check_type(const std::string& s)
  {
    if (std::regex_match(s, std::regex("[-|+]?[0-9]+")))
    {
      return Cod_value_type::INT;
    }
    else if (std::regex_match(s, std::regex("[-|+]?[0-9]+.[0-9]+")))
    {
      return Cod_value_type::FLOAT;
    }
    else
    {
      return Cod_value_type::STRING;
    }
  }

  std::string path;
  std::vector<std::string> cod_txt;
};
#endif