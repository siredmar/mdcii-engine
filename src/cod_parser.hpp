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
#include <experimental/optional>

#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/variant.hpp>

#include <google/protobuf/util/json_util.h>
#include "proto/cod.pb.h"

class Cod_Parser
{
public:
  Cod_Parser(std::string cod_file_path);


private:
  // Input/Output related functions
  bool decode();
  bool convert();
  bool json();

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
  // cod_pb::Variable get_variable(int index);
  std::experimental::optional<cod_pb::Variable*> get_variable(cod_pb::Object* obj, const std::string& name);
  int calculate_operation(int old_value, const std::string& operation, const std::string& op);

  // Object stack related functions
  bool top_is_number_object();
  void add_object_to_stack(cod_pb::Object* o, bool number_object, int spaces);
  bool object_finished();

  // String handling functions
  std::vector<std::string> regex_match(std::string regex, std::string str);
  std::vector<std::string> regex_search(std::string regex, std::string str);
  std::string tabs_to_spaces(const std::string& str);
  int count_front_spaces(const std::string& str);
  std::string trim_spaces_leading_trailing(const std::string& s);
  bool is_empty(const std::string& str);
  bool is_substring(std::string str, std::string substr);
  std::vector<std::string> split_by_delimiter(std::string str, std::string delim);
  std::string trim_comment_from_line(std::string str);
  bool begins_with(std::string str, std::string begin);

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

  // cod_pb::Variables variables;
  cod_pb::Variables constants;
  cod_pb::Objects objects;

  std::map<std::string, cod_pb::Object*> object_map;
  std::map<std::string, int> variable_map;
  std::map<int, cod_pb::Object*> object_id_map;
  cod_pb::Object* current_object = nullptr;
};
#endif