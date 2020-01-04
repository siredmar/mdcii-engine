
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

#ifndef COD_PARSER_HPP
#define COD_PARSER_HPP

#include <cstring>
#include <experimental/optional>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <stack>
#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/regex.hpp>
#include <boost/variant.hpp>

#include "proto/cod.pb.h"
#include <google/protobuf/util/json_util.h>

class Cod_Parser
{
public:
  Cod_Parser(const std::string& cod_file_path, bool decode, bool debug);
  Cod_Parser(const std::string& file_as_string);

  void debug_output();
  cod_pb::Objects objects;

private:
  // Input/Output related functions
  bool read_file_as_string(const std::string& buffer);
  bool read_file(bool decode);
  void parse_file();
  void json();

  // Object related functions
  cod_pb::Object* create_object(bool number_object, int spaces, bool add_to_stack);
  cod_pb::Object* create_or_reuse_object(const std::string& name, bool number_object, int spaces, bool add_to_stack);
  std::experimental::optional<cod_pb::Object*> get_object(const std::string& name);
  std::experimental::optional<cod_pb::Object*> get_object(int id);
  std::experimental::optional<cod_pb::Object*> get_sub_object(cod_pb::Object* obj, const std::string& name);
  cod_pb::Object* objfill_prefill(cod_pb::Object* obj);
  void reset_objfill_prefill();

  // Constants related functions
  int constant_exists(const std::string& key);
  cod_pb::Variable get_value(const std::string& key, const std::string& value, bool is_math, cod_pb::Variables variables);

  // Variables related functions
  int exists_in_current_object(const std::string& variable_name);
  cod_pb::Variable* create_or_reuse_variable(const std::string& name);
  cod_pb::Variable get_variable(const std::string& key);
  std::experimental::optional<cod_pb::Variable*> get_variable(cod_pb::Object* obj, const std::string& name);
  int calculate_operation(int old_value, const std::string& operation, int op);

  // Object stack related functions
  bool top_is_number_object();
  void add_object_to_stack(cod_pb::Object* o, bool number_object, int spaces);
  void object_finished();

  // String handling functions
  std::vector<std::string> regex_match(const std::string& regex, const std::string& str);
  std::vector<std::string> regex_search(const std::string& regex, const std::string& str);
  std::string tabs_to_spaces(const std::string& str);
  int count_front_spaces(const std::string& str);
  std::string trim_spaces_leading_trailing(const std::string& s);
  bool is_empty(const std::string& str);
  bool is_substring(const std::string& str, const std::string& substr);
  std::vector<std::string> split_by_delimiter(const std::string& str, const std::string& delim);
  std::string trim_comment_from_line(const std::string& str);
  bool begins_with(const std::string& str, const std::string& begin);

  struct ObjectType
  {
    cod_pb::Object* object;
    bool number_object = {false};
    int spaces = -1;
  };

  std::stack<ObjectType> object_stack;

  struct ObjFillRangeType
  {
    cod_pb::Object object;
    std::string start;
    std::string stop;
    int stacksize = 0;
    bool filling = false;
  };

  ObjFillRangeType ObjFill_range;

  enum class Cod_value_type
  {
    INT = 0,
    FLOAT,
    STRING
  };

  Cod_value_type check_type(const std::string& s);

  std::string path;
  std::vector<std::string> cod_txt;

  cod_pb::Variables constants;

  std::map<std::string, cod_pb::Object*> object_map;
  std::map<std::string, int> variable_map;
  std::map<int, cod_pb::Object*> object_id_map;
  cod_pb::Object* current_object = nullptr;
};
#endif
