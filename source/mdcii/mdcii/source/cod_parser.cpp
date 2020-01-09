
// This file is part of the MDCII Game Engine.
// Copyright (C) 2019  Armin Schlegel
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

#include "cod_parser.hpp"

Cod_Parser::Cod_Parser(const std::string& cod_file_path, bool decode, bool debug)
  : path(cod_file_path)
{
  read_file(decode);
  parse_file();
  if (debug == true)
  {
    debug_output();
  }
}

Cod_Parser::Cod_Parser(const std::string& file_as_string)
{
  read_file_as_string(file_as_string);
  parse_file();
}

bool Cod_Parser::read_file(bool decode)
{
  std::ifstream input(path, std::ios::binary);
  std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(input), {});

  if (decode == true)
  {
    for (auto& c : buffer)
    {
      c = -c;
    }
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

bool Cod_Parser::read_file_as_string(const std::string& buffer)
{
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

void Cod_Parser::parse_file()
{
  std::map<std::string, int> variable_numbers;
  std::map<std::string, std::vector<int>> variable_numbers_array;

  int spaces = -1;
  for (int line_index = 0; line_index < cod_txt.size(); line_index++)
  {
    std::string line = cod_txt[line_index];
    spaces = count_front_spaces(line);

    if (is_substring(line, "Nahrung:") || is_substring(line, "Soldat:") || is_substring(line, "Turm:"))
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
            int i = constant_exists(key);
            cod_pb::Variable* variable;
            if (i != -1)
            {
              variable = constants.mutable_variable(i);
            }
            else
            {
              variable = constants.add_variable();
            }
            variable->set_name(key);
            variable->set_value_string(std::to_string(number));
          }
        }
        else
        // example: 'IDHANDW =   20501'
        {
          int i = constant_exists(key);
          cod_pb::Variable* variable;
          if (i != -1)
          {
            variable = constants.mutable_variable(i);
          }
          else
          {
            variable = constants.add_variable();
          }
          *variable = get_value(key, value, is_math, constants);
        }
        continue;
      }
    }
    {
      // example: '@Pos:       +0, +42'
      std::vector<std::string> result = regex_search("@(\\w+):.*(,)", line);
      if (result.size() > 0)
      {
        std::string name = result[1];
        std::vector<int> offsets;
        result = regex_search(":\\s*(.*)", line);
        if (result.size() > 0)
        {

          std::vector<std::string> tokens = split_by_delimiter(result[1], ",");
          for (auto& e : tokens)
          {
            e = trim_spaces_leading_trailing(e);
            std::vector<std::string> number = regex_search("([+|-]?)(\\d+)", e);
            int offset = std::stoi(number[2]);
            if (number[1] == "-")
            {
              offset *= -1;
            }
            offsets.push_back(offset);
          }
        }
        int index = exists_in_current_object(name);
        std::vector<int> current_array_values;
        for (int i = 0; i < offsets.size(); i++)
        {
          int current_value = 0;
          if (index != -1)
          {
            current_value = current_object->variables().variable(index).value_array().value(i).value_int();
            current_value = calculate_operation(current_value, "+", offsets[i]);
            current_object->mutable_variables()->mutable_variable(index)->mutable_value_array()->mutable_value(i)->set_value_int(current_value);
          }
          else
          {
            current_value = calculate_operation(variable_numbers_array[name][i], "+", offsets[i]);
            auto var = create_or_reuse_variable(name);
            var->set_name(name);
            var->mutable_value_array()->add_value()->set_value_int(current_value);
          }
          current_array_values.push_back(current_value);
        }
        variable_numbers_array[name] = current_array_values;
        continue;
      }
    }
    {
      if (is_substring(line, ",") == true && is_substring(line, "ObjFill") == false)
      {
        // example:
        // Arr: 5, 6
        // Var: 10-Arr[0], 20+Arr[1]
        if (is_substring(line, "["))
        {
          std::vector<std::string> result = regex_search("(\\w+)\\s*:\\s*(.+)", line);
          if (result.size() > 0)
          {
            std::string name = result[1];
            auto var = create_or_reuse_variable(name);
            if (exists_in_current_object(name) == true)
            {
              var->mutable_value_array()->Clear();
            }
            var->set_name(name);
            std::string values = result[2];
            std::vector<std::string> split_values = split_by_delimiter(values, ",");
            for (auto v : split_values)
            {
              std::vector<std::string> tokens = regex_search("((\\d+)([+|-])(\\w+)\\[(\\d+)\\])", v);
              if (tokens.size() > 0)
              {
                int index = exists_in_current_object(tokens[4]);
                if (index != -1)
                {
                  int arr_value = current_object->variables().variable(index).value_array().value(std::stoi(tokens[5])).value_int();
                  int new_value = calculate_operation(std::stoi(tokens[2]), tokens[3], arr_value);
                  var->mutable_value_array()->add_value()->set_value_int(new_value);
                }
              }
            }
          }
        }
        else
        // example: Var: 1, 2, 3
        {
          std::vector<std::string> result = regex_search("(\\b(?!Objekt\\b)\\w+)\\s*:\\s*(.+)", line);
          if (result.size() > 0)
          {
            std::vector<int> current_array_values;
            std::vector<std::string> values = split_by_delimiter(result[2], ",");
            for (auto& v : values)
            {
              v = trim_spaces_leading_trailing(v);
            }
            bool var_exists = exists_in_current_object(result[1]);
            auto var = create_or_reuse_variable(result[1]);
            var->set_name(result[1]);
            auto arr = var->mutable_value_array();

            if (var_exists == true)
            {
              arr->Clear();
            }

            for (auto v : values)
            {
              if (check_type(v) == Cod_value_type::INT)
              {
                arr->add_value()->set_value_int(std::stoi(v));
                current_array_values.push_back(std::stoi(v));
              }
              else if (check_type(v) == Cod_value_type::FLOAT)
              {
                arr->add_value()->set_value_float(std::stof(v));
              }
              else
              {
                int i = constant_exists(v);
                if (i != -1)
                {
                  auto var = get_variable(v);
                  if (var.Value_case() == cod_pb::Variable::ValueCase::kValueInt)
                  {
                    arr->add_value()->set_value_int(var.value_int());
                    current_array_values.push_back(var.value_int());
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
            variable_numbers_array[result[1]] = current_array_values;
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
          auto var = variable_numbers[current_object->variables().variable(index).name()];
          current_value = calculate_operation(var, result[4], std::stoi(result[5]));
          current_object->mutable_variables()->mutable_variable(index)->set_value_int(current_value);
        }
        else
        {
          current_value = calculate_operation(variable_numbers[result[3]], result[4], std::stoi(result[5]));
          auto var = create_or_reuse_variable(result[3]);
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
        if (constant_exists(result[2]) != -1)
        {
          current_value = get_variable(result[2]).value_int();
        }
        if (current_value != -1)
        {
          current_value = calculate_operation(current_value, result[3], std::stoi(result[4]));
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
          if (result[1] == "Id")
          {
            object_id_map[std::stoi(result[2])] = current_object;
          }

          var->set_value_int(std::stoi(result[2]));
          variable_numbers[result[1]] = std::stoi(result[2]);
        }
        else
        {
          if (constant_exists(result[2]) != -1)
          {
            auto v = get_variable(result[2]);
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
      // example: Objekt: NAME
      std::vector<std::string> result = regex_search("Objekt:\\s*([\\w,]+)", line);
      if (result.size() > 0)
      {
        current_object = create_or_reuse_object(result[1], false, spaces, true);
        current_object->set_name(result[1]);
        object_map[result[1]] = current_object;
        continue;
      }
    }
    {
      // example: @Nummer: +1
      std::vector<std::string> result = regex_search("(Nummer):\\s*([+|-]?)(\\d+)", line);
      if (result.size() > 0)
      {
        if (top_is_number_object() == true)
        {
          object_finished();
        }

        int current_number = variable_numbers[result[1]];
        current_number = calculate_operation(current_number, result[2], std::stoi(result[3]));
        variable_numbers[result[1]] = current_number;
        std::string name = std::to_string(current_number);
        current_object = create_object(true, spaces, true);
        current_object->set_name(name);
        object_map[name] = current_object;
        if (name == ObjFill_range.stop || object_stack.size() < ObjFill_range.stacksize)
        {
          reset_objfill_prefill();
        }
        continue;
      }
    }
    {
      // example: EndObj
      std::vector<std::string> result = regex_search("\\s*EndObj", line);
      if (result.size() > 0)
      {
        if (object_stack.size() <= ObjFill_range.stacksize)
        {
          reset_objfill_prefill();
        }

        if (object_stack.size() > 0)
        {
          if (object_stack.top().spaces > spaces)
          {
            // finish previous number object
            object_finished();
          }
          object_finished();
        }
        continue;
      }
    }

    {
      std::vector<std::string> result = regex_search("ObjFill:\\s*(\\w+)[,]?\\s*(\\w+)*", line);
      if (result.size() > 0)
      {
        // Check if range object fill to insert to objects from start to stop
        // example: ObjFill: 0, MAX
        if (result[2] != "")
        {
          ObjFill_range.start = result[1];
          ObjFill_range.stop = result[2];
          ObjFill_range.object = *object_stack.top().object;
          ObjFill_range.filling = true;

          ObjFill_range.stacksize = object_stack.size();
          object_finished();
          current_object = object_stack.top().object;
          current_object->mutable_objects()->ReleaseLast();
        }
        else
        {
          // example: ObjFill: OBJ
          auto real_name = get_variable(result[1]);
          auto obj = get_object(real_name.value_string());
          if (obj)
          {
            if (obj.value()->has_variables())
            {
              for (int i = 0; i < obj.value()->variables().variable_size(); i++)
              {
                auto variable = create_or_reuse_variable(obj.value()->variables().variable(i).name());
                *variable = obj.value()->variables().variable(i);
              }
            }
            if (obj.value()->objects_size() > 0)
            {
              for (int i = 0; i < obj.value()->objects_size(); i++)
              {
                auto object = create_or_reuse_object(obj.value()->objects(i).name(), false, spaces, false);
                *object = obj.value()->objects(i);
              }
            }
          }
        }
        continue;
      }
      {
        // example: Nummer: 0
        std::vector<std::string> result = regex_search("Nummer:\\s*(\\w+)", line);
        if (result.size() > 0)
        {
          if (top_is_number_object() == true)
          {
            object_finished();
          }
          current_object = create_object(true, spaces, true);
          current_object->set_name(result[1]);
          object_map[result[1]] = current_object;
          continue;
        }
      }
    }
  }
  // std::cout << objects.DebugString() << std::endl;
  // std::cout << variables.DebugString() << std::endl;
}

void Cod_Parser::json()
{
  std::string json_string;
  google::protobuf::util::JsonPrintOptions options;
  options.add_whitespace = true;
  options.always_print_primitive_fields = true;
  MessageToJsonString(objects, &json_string, options);
  std::cout << json_string << std::endl;
}

cod_pb::Object* Cod_Parser::create_object(bool number_object, int spaces, bool add_to_stack)
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
  if (add_to_stack == true)
  {
    object_stack.push(obj);
  }
  if (ObjFill_range.filling == true && number_object == true)
  {
    ret = objfill_prefill(obj.object);
  }
  return ret;
}

cod_pb::Object* Cod_Parser::create_or_reuse_object(const std::string& name, bool number_object, int spaces, bool add_to_stack)
{
  if (current_object)
  {
    auto optional_obj = get_sub_object(current_object, name);
    if (optional_obj)
    {
      auto o = optional_obj.value();
      if (add_to_stack == true)
      {
        add_object_to_stack(o, number_object, spaces);
      }
      return o;
    }
  }
  return create_object(number_object, spaces, add_to_stack);
}

std::experimental::optional<cod_pb::Object*> Cod_Parser::get_object(const std::string& name)
{
  if (object_map.count(name))
  {
    return object_map[name];
  }
  return {};
}

std::experimental::optional<cod_pb::Object*> Cod_Parser::get_object(int id)
{
  if (object_id_map.count(id))
  {
    return object_id_map[id];
  }
  return {};
}

std::experimental::optional<cod_pb::Object*> Cod_Parser::get_sub_object(cod_pb::Object* obj, const std::string& name)
{
  for (int i = 0; i < obj->objects_size(); i++)
  {
    if (obj->objects(i).name() == name)
    {
      return obj->mutable_objects(i);
    }
  }
  return {};
}

cod_pb::Object* Cod_Parser::objfill_prefill(cod_pb::Object* obj)
{
  std::string name = obj->name();
  *obj = ObjFill_range.object;
  obj->set_name(name);
  return obj;
}

void Cod_Parser::reset_objfill_prefill()
{
  ObjFill_range.start = "";
  ObjFill_range.stop = "";
  ObjFill_range.filling = false;
  ObjFill_range.stacksize = 0;
}

int Cod_Parser::constant_exists(const std::string& key)
{
  for (int i = 0; i < constants.variable_size(); i++)
  {
    if (constants.variable(i).name() == key)
    {
      return i;
    }
  }
  return -1;
}

cod_pb::Variable Cod_Parser::get_value(const std::string& key, const std::string& value, bool is_math, cod_pb::Variables variables)
{
  cod_pb::Variable ret;
  ret.set_name(key);
  if (is_math == true)
  {
    // Searching for some characters followed by a + or - sign and some digits.
    // example: VALUE+20
    std::vector<std::string> result = regex_search("(\\w+)(\\+|-)(\\d+)", value);
    if (result.size() > 0)
    {
      cod_pb::Variable old_val;
      int i = constant_exists(result[1]);
      if (i != -1)
      {
        old_val = variables.variable(i);
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
      if (constant_exists(key) == true)
      {
        auto v = get_variable(result[1]);
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

// Variables related functions
int Cod_Parser::exists_in_current_object(const std::string& variable_name)
{
  int index = -1;
  if (current_object)
  {
    // Check if variable already exists in current_object (e.g. copied from ObjFill)
    for (index = 0; index < current_object->variables().variable_size(); index++)
    {
      if (current_object->variables().variable(index).name() == variable_name)
      {
        return index;
      }
    }
  }
  return -1;
}

cod_pb::Variable* Cod_Parser::create_or_reuse_variable(const std::string& name)
{
  if (current_object)
  {
    auto optional_var = get_variable(current_object, name);
    if (optional_var)
    {
      return optional_var.value();
    }
  }
  return current_object->mutable_variables()->add_variable();
}

cod_pb::Variable Cod_Parser::get_variable(const std::string& key)
{
  for (int i = 0; i < constants.variable_size(); i++)
  {
    if (constants.variable(i).name() == key)
    {
      return constants.variable(i);
    }
  }
  cod_pb::Variable ret;
  return ret;
}

std::experimental::optional<cod_pb::Variable*> Cod_Parser::get_variable(cod_pb::Object* obj, const std::string& name)
{
  for (int i = 0; i < obj->variables().variable_size(); i++)
  {
    if (obj->variables().variable(i).name() == name)
    {
      return obj->mutable_variables()->mutable_variable(i);
    }
  }
  return {};
}

int Cod_Parser::calculate_operation(int old_value, const std::string& operation, int op)
{
  int current_value = old_value;
  if (operation == "+")
  {
    current_value += op;
  }
  else if (operation == "-")
  {
    current_value -= op;
  }
  else
  {
    current_value = op;
  }
  return current_value;
}


// Object stack related functions
bool Cod_Parser::top_is_number_object()
{
  if (!object_stack.empty())
  {
    return object_stack.top().number_object;
  }
  return false;
}

void Cod_Parser::add_object_to_stack(cod_pb::Object* o, bool number_object, int spaces)
{
  ObjectType obj;
  obj.object = o;
  obj.number_object = number_object;
  obj.spaces = spaces;
  object_stack.push(obj);
}

void Cod_Parser::object_finished()
{
  if (object_stack.size() > 0)
  {
    object_stack.pop();
  }
}

// String handling functions
std::vector<std::string> Cod_Parser::regex_match(const std::string& regex, const std::string& str)
{
  std::vector<std::string> ret;
  boost::regex expr {regex};
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

std::vector<std::string> Cod_Parser::regex_search(const std::string& regex, const std::string& str)
{
  std::vector<std::string> ret;
  boost::regex expr {regex};
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


std::string Cod_Parser::tabs_to_spaces(const std::string& str)
{
  std::string newtext = "  ";
  boost::regex re("\t");

  std::string result = boost::regex_replace(str, re, newtext);
  return result;
}

int Cod_Parser::count_front_spaces(const std::string& str)
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

std::string Cod_Parser::trim_spaces_leading_trailing(const std::string& s)
{
  std::string input = s;
  boost::algorithm::trim(input);
  return input;
}

bool Cod_Parser::is_empty(const std::string& str)
{
  if (str.size() == 0 || std::all_of(str.begin(), str.end(), isspace))
  {
    return true;
  }
  return false;
}

bool Cod_Parser::is_substring(const std::string& str, const std::string& substr)
{
  std::size_t found = str.find(substr);
  if (found != std::string::npos)
  {
    return true;
  }
  return false;
}

std::vector<std::string> Cod_Parser::split_by_delimiter(const std::string& str, const std::string& delim)
{
  std::vector<std::string> tokens;
  boost::split(tokens, str, boost::is_any_of(delim));
  return tokens;
}

std::string Cod_Parser::trim_comment_from_line(const std::string& str)
{
  return split_by_delimiter(str, ";")[0];
}

bool Cod_Parser::begins_with(const std::string& str, const std::string& begin)
{
  if (str.rfind(begin, 0) == 0)
  {
    return true;
  }
  return false;
}

Cod_Parser::Cod_value_type Cod_Parser::check_type(const std::string& s)
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

void Cod_Parser::debug_output()
{
  std::cout << objects.DebugString() << std::endl;
}
