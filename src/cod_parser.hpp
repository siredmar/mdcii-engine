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
  cod_pb::Variables variables;
  cod_pb::Objects objects;
  std::map<std::string, cod_pb::Object*> object_map;
  cod_pb::Object* current_object;
  struct ObjectType
  {
    cod_pb::Object* object;
    bool number_object = {false};
    int spaces = -1;
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

  cod_pb::Object* Create_object(bool number_object, int spaces)
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
    obj.spaces = spaces;
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
    cod_pb::Map gfx_map;

    std::map<std::string, int> variable_numbers;

    int spaces = -1;

    for (int line_index = 0; line_index < cod_txt.size() - 1; line_index++)
    {
      if (is_substring(cod_txt[line_index], "Stirbtime:	2.5"))
      {
        std::cout << cod_txt[line_index] << std::endl;
      }

      std::string line = cod_txt[line_index];
      std::string next_line = cod_txt[line_index + 1];
      spaces = count_front_spaces(line);

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
          std::string key = result[2];
          std::string value = result[3];
          // example: 'HAUSWACHS = Nummer'
          if (value == "Nummer")
          {
            if (variable_numbers.count(value))
            {
              int number = variable_numbers[value];
              int i = exists(key);
              cod_pb::Variable* variable;
              if (i != -1)
              {
                variable = variables.mutable_variable(i);
              }
              else
              {
                variable = variables.add_variable();
              }
              variable->set_name(key);
              variable->set_value_string(std::to_string(number));
            }
          }
          else
          // example: 'IDHANDW =   20501'
          {
            int i = exists(key);
            cod_pb::Variable* variable;
            if (i != -1)
            {
              auto var = variables.mutable_variable(i);
              *var = get_value(key, value, is_math, variables);
            }
            else
            {
              auto variable = variables.add_variable();
              *variable = get_value(key, value, is_math, variables);
            }
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
                if (exists(v) != -1)
                {
                  int i = exists(v);
                  cod_pb::Variable* variable;
                  auto var = get_value(v);
                  if (var.Value_case() == cod_pb::Variable::ValueCase::kValueInt)
                  {
                    arr->add_value()->set_value_int(var.value_int());
                  }
                  else if (var.Value_case() == cod_pb::Variable::ValueCase::kValueFloat)
                  {
                    arr->add_value()->set_value_float(var.value_float());
                  }
                  else
                  {
                    arr->add_value()->set_value_string(var.value_string());
                  }
                }
                else
                {
                  arr->add_value()->set_value_string(v);
                }
              }
            }
          }
          continue;
        }
      }
      {
        // example: '@Gfx:       -36'
        std::vector<std::string> result = regex_search("((@)(\\b(?!Nummer\\b)\\w+))\\s*:\\s*([+|-]?)(\\d+)", line);
        if (result.size() > 0)
        {
          int index = exists_in_current_object(result[3]);

          int current_value = 0;
          if (index != -1)
          {
            current_value = calculate_operation(current_object->variables().variable(index).value_int(), result[4], result[5]);
            current_object->mutable_variables()->mutable_variable(index)->set_value_int(current_value);
          }
          else
          {
            current_value = calculate_operation(variable_numbers[result[3]], result[4], result[5]);
            auto var = current_object->mutable_variables()->add_variable();
            var->set_name(result[3]);
            var->set_value_int(current_value);
          }
          variable_numbers[result[3]] = current_value;
          continue;
        }
      }
      {
        // example: 'Gfx:        GFXBODEN+80'
        std::vector<std::string> result = regex_search("(\\b(?!Nummer\\b)\\w+)\\s*:\\s*(\\w+)\\s*([+|-])\\s*(\\d+)", line);
        if (result.size() > 0)
        {
          int index = exists_in_current_object(result[1]);

          int current_value = -1;
          if (exists(result[2]))
          {
            current_value = get_value(result[2]).value_int();
          }
          if (current_value != -1)
          {
            current_value = calculate_operation(current_value, result[3], result[4]);
          }
          else
          {
            current_value = 0;
          }
          variable_numbers[result[1]] = current_value;

          if (index != -1)
          {
            current_object->mutable_variables()->mutable_variable(index)->set_value_int(current_value);
          }
          else
          {
            auto var = current_object->mutable_variables()->add_variable();
            var->set_name(result[1]);
            var->set_value_int(current_value);
          }
          continue;
        }
      }
      {
        // example: 'Rotate: 1' or  'Gfx:        GFXGALGEN'
        std::vector<std::string> result = regex_search("(\\b(?!Objekt|ObjFill|Nummer\\b)\\w+)\\s*:\\s*(\\w+)", line);
        if (result.size() > 0)
        {
          int index = exists_in_current_object(result[1]);

          cod_pb::Variable* var;
          if (index != -1)
          {
            var = current_object->mutable_variables()->mutable_variable(index);
          }
          else
          {
            var = current_object->mutable_variables()->add_variable();
            var->set_name(result[1]);
          }

          if (check_type(result[2]) == Cod_value_type::INT)
          {
            var->set_value_int(std::stoi(result[2]));
          }
          else
          {
            if (exists(result[2]) != -1)
            {
              auto v = get_value(result[2]);
              var->set_value_int(v.value_int());
            }
            else
            {
              var->set_value_string(result[2]);
            }
          }
          continue;
        }
      }
      {
        std::vector<std::string> result = regex_search("Objekt:\\s*([\\w,]+)", line);
        if (result.size() > 0)
        {
          current_object = Create_object(false, spaces);
          current_object->set_name(result[1]);
          object_map[result[1]] = current_object;
          continue;
        }
      }
      {
        std::vector<std::string> result = regex_search("(Nummer):\\s*([+|-]?)(\\d+)", line);
        if (result.size() > 0)
        {
          if (Top_is_number_object() == true)
          {
            Object_finished();
          }

          int current_number = variable_numbers[result[1]];
          current_number = calculate_operation(current_number, result[2], result[3]);
          variable_numbers[result[1]] = current_number;
          current_object = Create_object(true, spaces);
          current_object->set_name(std::to_string(current_number));
          object_map[std::to_string(current_number)] = current_object;
          continue;
        }
      }
      {
        std::vector<std::string> result = regex_search("\\s*EndObj", line);
        if (result.size() > 0)
        {
          if (object_stack.top().spaces > spaces)
          {
            // finish previous number object
            Object_finished();
          }
          // std::vector<std::string> result_next = regex_search("Nummer:", next_line);
          // if (result_next.size() > 0)
          // {
          //   if (Top_is_number_object() == true)
          //   {
          //     Object_finished();
          //   }
          // }
          Object_finished();
          continue;
        }
      }

      // {
      //   std::vector<std::string> result = regex_search("Nummer\\s*:", next_line);
      //   if (result.size() > 0)
      //   {
      //     if (Top_is_number_object() == true)
      //     {
      //       Object_finished();
      //     }
      //     continue;
      //   }
      // }
      {
        std::vector<std::string> result = regex_search("ObjFill:\\s*([\\w,]+)", line);
        if (result.size() > 0)
        {
          cod_pb::Object obj;
          auto real_name = get_value(result[1]);
          if (get_object(real_name.value_string(), obj) == true)
          {
            if (obj.has_variables())
            {
              for (int i = 0; i < obj.variables().variable_size(); i++)
              {
                auto variable = current_object->mutable_variables()->add_variable();
                *variable = obj.variables().variable(i);
              }
            }
            if (obj.objects_size() > 0)
            {
              for (int i = 0; i < obj.objects_size(); i++)
              {
                auto object = current_object->add_objects();
                *object = obj.objects(i);
              }
            }
          }
          continue;
        }
        {
          // TODO: get the constants value
          std::vector<std::string> result = regex_search("Nummer:\\s*(\\w+)", line);
          if (result.size() > 0)
          {
            if (Top_is_number_object() == true)
            {
              Object_finished();
            }
            current_object = Create_object(true, spaces);
            current_object->set_name(result[1]);
            object_map[result[1]] = current_object;
            continue;
          }
        }
      }
    }
    std::cout << objects.DebugString() << std::endl;
    std::cout << variables.DebugString() << std::endl;
  }

  int exists_in_current_object(const std::string& variable_name)
  {
    int index = -1;
    // Check if variable already exists in current_object (e.g. copied from ObjFill)
    for (index = 0; index < current_object->variables().variable_size(); index++)
    {
      if (current_object->variables().variable(index).name() == variable_name)
      {
        return index;
      }
    }
    return -1;
  }

  int calculate_operation(int old_value, const std::string& operation, const std::string& op)
  {
    int current_value = old_value;
    if (operation == "+")
    {
      current_value += std::stoi(op);
    }
    else if (operation == "-")
    {
      current_value -= std::stoi(op);
    }
    else
    {
      current_value = std::stoi(op);
    }
    return current_value;
  }

  bool get_object(const std::string& name, cod_pb::Object& ret)
  {
    if (object_map.count(name))
    {
      ret = *object_map[name];
      return true;
    }
    return false;
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
        int i = exists(result[1]);
        if (i != -1)
        {
          old_val = variables.variable(i); // get_value(result[1]);
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
        if (exists(key) == true)
        {
          auto v = get_value(result[1]);
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

    ret.set_value_int(0);
    return ret;
  }

  std::string tabs_to_spaces(const std::string& str)
  {
    std::string newtext = "  ";
    boost::regex re("\t");

    std::string result = boost::regex_replace(str, re, newtext);
    return result;
  }

  int count_front_spaces(const std::string& str)
  {
    int number_of_spaces = 0;
    std::vector<std::string> result = regex_search("(\\s*)(\\w+)", str);
    if (result.size() > 0)
    {
      for (auto& iter : result[1])
      {
        if (iter == ' ')
        {
          number_of_spaces++;
        }
        if (iter != ' ')
        {
          break;
        }
      }
    }
    return number_of_spaces;
  }

  int exists(const std::string& key)
  {
    for (int i = 0; i < variables.variable_size(); i++)
    {
      if (variables.variable(i).name() == key)
      {
        return i;
      }
    }
    return -1;
  }

  cod_pb::Variable get_value(const std::string& key)
  {
    for (int i = 0; i < variables.variable_size(); i++)
    {
      if (variables.variable(i).name() == key)
      {
        return variables.variable(i);
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
        line = trim_comment_from_line(tabs_to_spaces((line)));
        if (is_empty(line) == false)
        {
          cod_txt.push_back(line);
        }
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
    // TODO: replace with boost::regex to get rid of std::regex
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