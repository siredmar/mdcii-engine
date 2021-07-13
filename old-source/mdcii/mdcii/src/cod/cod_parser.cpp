
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

#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/regex.hpp>
#include <boost/variant.hpp>

#include <google/protobuf/util/json_util.h>

#include "cod/cod_parser.hpp"
#include "cod/codhelpers.hpp"

#include "version/version.hpp"

namespace fs
    = std::filesystem;

CodParser::CodParser(const std::string& codFilePath, bool decode, bool debug)
    : path(codFilePath)
    , cache(std::make_unique<CacheProtobuf<cod_pb::Objects>>("mdcii/" + Version::GameVersionString() + "/" + fs::path(codFilePath).filename().string() + ".json"))
{
    if (cache->Exists())
    {
        objects = cache->Deserialize();
    }
    else
    {
        ReadFile(decode);
        ParseFile();
        if (debug == true)
        {
            DebugOutput();
        }
        cache->Serialize(objects);
    }
}

CodParser::CodParser(const std::string& fileAsString)
{
    codTxt = ReadFileAsString(fileAsString);
    ParseFile();
}

bool CodParser::ReadFile(bool decode)
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
    for (unsigned int i = 0; i < buffer.size() - 1; i++)
    {
        if (buffer[i + 1] != '\n' && buffer[i] != '\r')
        {
            line.append(1, buffer[i]);
        }
        else
        {
            line = TrimCommentFromLine(TabsToSpaces((line)));
            if (IsEmpty(line) == false)
            {
                codTxt.push_back(line);
            }
            line = "";
            i++; // hop over '\n'
        }
    }
    return true;
}

void CodParser::ParseFile()
{
    std::map<std::string, int> variableNumbers;
    std::map<std::string, std::vector<int>> variableNumbersArray;

    for (unsigned int lineIndex = 0; lineIndex < codTxt.size(); lineIndex++)
    {
        std::string line = codTxt[lineIndex];
        int spaces = CountFrontSpaces(line);

        if (IsSubstring(line, "Nahrung:") || IsSubstring(line, "Soldat:") || IsSubstring(line, "Turm:"))
        {
            // TODO : skipped for now
            continue;
        }

        {
            // constant assignment
            std::vector<std::string> result = RegexSearch("(@?)(\\w+)\\s*=\\s*((?:\\d+|\\+|\\w+)+)", line);
            if (result.size() > 0)
            {
                bool isMath = IsSubstring(result[3], "+");
                std::string key = result[2];
                std::string value = result[3];

                // example: 'HAUSWACHS = Nummer'
                if (value == "Nummer")
                {
                    if (variableNumbers.count(value))
                    {
                        int number = variableNumbers[value];
                        int i = ConstantExists(key);
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
                    int i = ConstantExists(key);
                    cod_pb::Variable* variable;
                    if (i != -1)
                    {
                        variable = constants.mutable_variable(i);
                    }
                    else
                    {
                        variable = constants.add_variable();
                    }
                    *variable = GetValue(key, value, isMath, constants);
                }
                continue;
            }
        }
        {
            // example: '@Pos:       +0, +42'
            std::vector<std::string> result = RegexSearch("@(\\w+):.*(,)", line);
            if (result.size() > 0)
            {
                std::string name = result[1];
                std::vector<int> offsets;
                result = RegexSearch(":\\s*(.*)", line);
                if (result.size() > 0)
                {
                    std::vector<std::string> tokens = SplitByDelimiter(result[1], ",");
                    for (auto& e : tokens)
                    {
                        e = TrimSpacesLeadingTrailing(e);
                        std::vector<std::string> number = RegexSearch("([+|-]?)(\\d+)", e);
                        int offset = std::stoi(number[2]);
                        if (number[1] == "-")
                        {
                            offset *= -1;
                        }
                        offsets.push_back(offset);
                    }
                }
                int index = ExistsInCurrentObject(name);
                std::vector<int> currentArrayValues;
                for (unsigned int i = 0; i < offsets.size(); i++)
                {
                    int currentValue = 0;
                    if (index != -1)
                    {
                        currentValue = variableNumbersArray[currentObject->variables().variable(index).name()][i];
                        currentValue = CalculateOperation(currentValue, "+", offsets[i]);
                        currentObject->mutable_variables()->mutable_variable(index)->mutable_value_array()->mutable_value(i)->set_value_int(currentValue);
                    }
                    else
                    {
                        currentValue = CalculateOperation(variableNumbersArray[name][i], "+", offsets[i]);
                        auto var = CreateOrReuseVariable(name);
                        var->set_name(name);
                        var->mutable_value_array()->add_value()->set_value_int(currentValue);
                    }
                    currentArrayValues.push_back(currentValue);
                }
                variableNumbersArray[name] = currentArrayValues;
                continue;
            }
        }
        {
            if (IsSubstring(line, ",") == true && IsSubstring(line, "ObjFill") == false)
            {
                // example:
                // Arr: 5, 6
                // Var: 10-Arr[0], 20+Arr[1]
                if (IsSubstring(line, "["))
                {
                    std::vector<std::string> result = RegexSearch("(\\w+)\\s*:\\s*(.+)", line);
                    if (result.size() > 0)
                    {
                        std::string name = result[1];
                        auto var = CreateOrReuseVariable(name);
                        if (ExistsInCurrentObject(name) == true)
                        {
                            var->mutable_value_array()->Clear();
                        }
                        var->set_name(name);
                        std::string values = result[2];
                        std::vector<std::string> splitValues = SplitByDelimiter(values, ",");
                        for (auto v : splitValues)
                        {
                            std::vector<std::string> tokens = RegexSearch("((\\d+)([+|-])(\\w+)\\[(\\d+)\\])", v);
                            if (tokens.size() > 0)
                            {
                                int index = ExistsInCurrentObject(tokens[4]);
                                if (index != -1)
                                {
                                    int arrayValue = currentObject->variables().variable(index).value_array().value(std::stoi(tokens[5])).value_int();
                                    int newValue = CalculateOperation(std::stoi(tokens[2]), tokens[3], arrayValue);
                                    var->mutable_value_array()->add_value()->set_value_int(newValue);
                                }
                            }
                        }
                    }
                }
                else
                // example: Var: 1, 2, 3
                {
                    std::vector<std::string> result = RegexSearch("(\\b(?!Objekt\\b)\\w+)\\s*:\\s*(.+)", line);
                    if (result.size() > 0)
                    {
                        if (objectStack.size() == 0)
                        {
                            unparsedLines.push_back(line);
                            continue;
                        }

                        std::vector<int> currentArrayValues;
                        std::vector<std::string> values = SplitByDelimiter(result[2], ",");
                        for (auto& v : values)
                        {
                            v = TrimSpacesLeadingTrailing(v);
                        }
                        bool varExists = ExistsInCurrentObject(result[1]);
                        auto var = CreateOrReuseVariable(result[1]);
                        var->set_name(result[1]);
                        auto arr = var->mutable_value_array();

                        if (varExists == true)
                        {
                            arr->Clear();
                        }

                        for (auto v : values)
                        {
                            if (CheckType(v) == CodValueType::INT)
                            {
                                arr->add_value()->set_value_int(std::stoi(v));
                                currentArrayValues.push_back(std::stoi(v));
                            }
                            else if (CheckType(v) == CodValueType::FLOAT)
                            {
                                arr->add_value()->set_value_float(std::stof(v));
                            }
                            else
                            {
                                int i = ConstantExists(v);
                                if (i != -1)
                                {
                                    auto var = GetVariable(v);
                                    if (var.Value_case() == cod_pb::Variable::ValueCase::kValueInt)
                                    {
                                        arr->add_value()->set_value_int(var.value_int());
                                        currentArrayValues.push_back(var.value_int());
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
                        variableNumbersArray[result[1]] = currentArrayValues;
                    }
                }
                continue;
            }
        }
        {
            // example: '@Gfx:       -36'
            std::vector<std::string> result = RegexSearch("((@)(\\b(?!Nummer\\b)\\w+))\\s*:\\s*([+|-]?)(\\d+)", line);
            if (result.size() > 0)
            {
                int index = ExistsInCurrentObject(result[3]);

                int currentValue = 0;
                if (index != -1)
                {
                    auto var = variableNumbers[currentObject->variables().variable(index).name()];
                    currentValue = CalculateOperation(var, result[4], std::stoi(result[5]));
                    currentObject->mutable_variables()->mutable_variable(index)->set_value_int(currentValue);
                }
                else
                {
                    currentValue = CalculateOperation(variableNumbers[result[3]], result[4], std::stoi(result[5]));
                    auto var = CreateOrReuseVariable(result[3]);
                    var->set_name(result[3]);
                    var->set_value_int(currentValue);
                }
                variableNumbers[result[3]] = currentValue;
                continue;
            }
        }
        {
            // example: 'Gfx:        GFXBODEN+80'
            std::vector<std::string> result = RegexSearch("(\\b(?!Nummer\\b)\\w+)\\s*:\\s*(\\w+)\\s*([+|-])\\s*(\\d+)", line);
            if (result.size() > 0)
            {
                int index = ExistsInCurrentObject(result[1]);

                int currentValue = -1;
                if (ConstantExists(result[2]) != -1)
                {
                    currentValue = GetVariable(result[2]).value_int();
                }
                if (currentValue != -1)
                {
                    currentValue = CalculateOperation(currentValue, result[3], std::stoi(result[4]));
                }
                else
                {
                    currentValue = 0;
                }
                variableNumbers[result[1]] = currentValue;

                if (index != -1)
                {
                    currentObject->mutable_variables()->mutable_variable(index)->set_value_int(currentValue);
                }
                else
                {
                    auto var = currentObject->mutable_variables()->add_variable();
                    var->set_name(result[1]);
                    var->set_value_int(currentValue);
                }
                continue;
            }
        }
        {
            // example: 'Rotate: 1' or  'Gfx:        GFXGALGEN'
            std::vector<std::string> result = RegexSearch("(\\b(?!Objekt|ObjFill|Nummer\\b)\\w+)\\s*:\\s*(\\w+)", line);
            if (result.size() > 0)
            {
                if (objectStack.size() == 0)
                {
                    unparsedLines.push_back(line);
                    continue;
                }

                int index = ExistsInCurrentObject(result[1]);

                cod_pb::Variable* var;
                if (index != -1)
                {
                    var = currentObject->mutable_variables()->mutable_variable(index);
                }
                else
                {
                    var = currentObject->mutable_variables()->add_variable();
                    var->set_name(result[1]);
                }

                if (CheckType(result[2]) == CodValueType::INT)
                {
                    if (result[1] == "Id")
                    {
                        objectIdMap[std::stoi(result[2])] = currentObject;
                    }

                    var->set_value_int(std::stoi(result[2]));
                    variableNumbers[result[1]] = std::stoi(result[2]);
                }
                else
                {
                    if (ConstantExists(result[2]) != -1)
                    {
                        auto v = GetVariable(result[2]);
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
            std::vector<std::string> result = RegexSearch("Objekt:\\s*([\\w,]+)", line);
            if (result.size() > 0)
            {
                currentObject = CreateOrReuseObject(result[1], false, spaces, true);
                currentObject->set_name(result[1]);
                objectMap[result[1]] = currentObject;
                continue;
            }
        }
        {
            // example: @Nummer: +1
            std::vector<std::string> result = RegexSearch("(Nummer):\\s*([+|-]?)(\\d+)", line);
            if (result.size() > 0)
            {
                if (TopIsNumberObject() == true)
                {
                    ObjectFinished();
                }

                int currentNumber = variableNumbers[result[1]];
                currentNumber = CalculateOperation(currentNumber, result[2], std::stoi(result[3]));
                variableNumbers[result[1]] = currentNumber;
                std::string name = std::to_string(currentNumber);
                currentObject = CreateObject(true, spaces, true);
                currentObject->set_name(name);
                objectMap[name] = currentObject;
                if ((name == objFillRange.stop) || (objectStack.size() < objFillRange.stacksize))
                {
                    ResetObjfillPrefill();
                }
                continue;
            }
        }
        {
            // example: EndObj
            std::vector<std::string> result = RegexSearch("\\s*EndObj", line);
            if (result.size() > 0)
            {
                if (objectStack.size() <= objFillRange.stacksize)
                {
                    ResetObjfillPrefill();
                }

                if (objectStack.size() > 0)
                {
                    if (objectStack.top().spaces > spaces)
                    {
                        // finish previous number object
                        ObjectFinished();
                    }
                    ObjectFinished();
                }
                continue;
            }
        }

        {
            std::vector<std::string> result = RegexSearch("ObjFill:\\s*(\\w+)[,]?\\s*(\\w+)*", line);
            if (result.size() > 0)
            {
                // Check if range object fill to insert to objects from start to stop
                // example: ObjFill: 0, MAX
                if (result[2] != "")
                {
                    objFillRange.start = result[1];
                    objFillRange.stop = result[2];
                    objFillRange.object = *objectStack.top().object;
                    objFillRange.filling = true;

                    objFillRange.stacksize = objectStack.size();
                    ObjectFinished();
                    currentObject = objectStack.top().object;
                    currentObject->mutable_objects()->ReleaseLast();
                }
                else
                {
                    // example: ObjFill: OBJ
                    auto realName = GetVariable(result[1]);
                    auto obj = GetObject(realName.value_string());
                    if (obj)
                    {
                        if (obj.value()->has_variables())
                        {
                            for (int i = 0; i < obj.value()->variables().variable_size(); i++)
                            {
                                auto variable = CreateOrReuseVariable(obj.value()->variables().variable(i).name());
                                *variable = obj.value()->variables().variable(i);
                            }
                        }
                        if (obj.value()->objects_size() > 0)
                        {
                            for (int i = 0; i < obj.value()->objects_size(); i++)
                            {
                                auto object = CreateOrReuseObject(obj.value()->objects(i).name(), false, spaces, false);
                                *object = obj.value()->objects(i);
                            }
                        }
                    }
                }
                continue;
            }
            {
                // example: Nummer: 0
                std::vector<std::string> result = RegexSearch("Nummer:\\s*(\\w+)", line);
                if (result.size() > 0)
                {
                    if (TopIsNumberObject() == true)
                    {
                        ObjectFinished();
                    }
                    currentObject = CreateObject(true, spaces, true);
                    currentObject->set_name(result[1]);
                    objectMap[result[1]] = currentObject;
                    continue;
                }
            }
        }
    }
    // std::cout << objects.DebugString() << std::endl;
    // std::cout << variables.DebugString() << std::endl;
}

void CodParser::Json()
{
    std::string jsonString;
    google::protobuf::util::JsonPrintOptions options;
    options.add_whitespace = true;
    options.always_print_primitive_fields = true;
    MessageToJsonString(objects, &jsonString, options);
    std::cout << jsonString << std::endl;
}

cod_pb::Object* CodParser::CreateObject(bool numberObject, int spaces, bool addToStack)
{
    cod_pb::Object* ret;
    if (objectStack.empty())
    {
        ret = objects.add_object();
    }
    else
    {
        ret = objectStack.top().object->add_objects();
    }
    ObjectType obj;
    obj.object = ret;
    obj.numberObject = numberObject;
    obj.spaces = spaces;
    if (addToStack == true)
    {
        objectStack.push(obj);
    }
    if (objFillRange.filling == true && numberObject == true)
    {
        ret = ObjfillPrefill(obj.object);
    }
    return ret;
}

cod_pb::Object* CodParser::CreateOrReuseObject(const std::string& name, bool numberObject, int spaces, bool addToStack)
{
    if (currentObject)
    {
        auto optionalObj = GetSubObject(currentObject, name);
        if (optionalObj)
        {
            auto o = optionalObj.value();
            if (addToStack == true)
            {
                AddToStack(o, numberObject, spaces);
            }
            return o;
        }
    }
    return CreateObject(numberObject, spaces, addToStack);
}

std::experimental::optional<cod_pb::Object*> CodParser::GetObject(const std::string& name)
{
    if (objectMap.count(name))
    {
        return objectMap[name];
    }
    return {};
}

std::experimental::optional<cod_pb::Object*> CodParser::GetObject(int id)
{
    if (objectIdMap.count(id))
    {
        return objectIdMap[id];
    }
    return {};
}

std::experimental::optional<cod_pb::Object*> CodParser::GetSubObject(cod_pb::Object* obj, const std::string& name)
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

cod_pb::Object* CodParser::ObjfillPrefill(cod_pb::Object* obj)
{
    std::string name = obj->name();
    *obj = objFillRange.object;
    obj->set_name(name);
    return obj;
}

void CodParser::ResetObjfillPrefill()
{
    objFillRange.start = "";
    objFillRange.stop = "";
    objFillRange.filling = false;
    objFillRange.stacksize = 0;
}

int CodParser::ConstantExists(const std::string& key)
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

cod_pb::Variable CodParser::GetValue(const std::string& key, const std::string& value, bool isMath, cod_pb::Variables variables)
{
    cod_pb::Variable ret;
    ret.set_name(key);
    if (isMath == true)
    {
        // Searching for some characters followed by a + or - sign and some digits.
        // example: VALUE+20
        std::vector<std::string> result = RegexSearch("(\\w+)(\\+|-)(\\d+)", value);
        if (result.size() > 0)
        {
            cod_pb::Variable oldValue;
            int i = ConstantExists(result[1]);
            if (i != -1)
            {
                oldValue = variables.variable(i);
            }
            else
            {
                oldValue.set_value_int(0);
            }

            if (oldValue.Value_case() == cod_pb::Variable::ValueCase::kValueString)
            {
                if (oldValue.value_string() == "RUINE_KONTOR_1")
                {
                    // TODO
                    oldValue.set_value_int(424242);
                }
            }
            if (result[2] == "+")
            {
                if (oldValue.Value_case() == cod_pb::Variable::ValueCase::kValueInt)
                {
                    ret.set_value_int(oldValue.value_int() + std::stoi(result[3]));
                    return ret;
                }
                else if (oldValue.Value_case() == cod_pb::Variable::ValueCase::kValueFloat)
                {
                    ret.set_value_int(oldValue.value_float() + std::stof(result[3]));
                    return ret;
                }
                else
                {
                    std::string o = oldValue.value_string();
                    ret.set_value_int(std::stoi(o) + std::stoi(result[3]));
                    return ret;
                }
            }

            if (result[2] == "-")
            {
                if (oldValue.Value_case() == cod_pb::Variable::ValueCase::kValueInt)
                {
                    ret.set_value_int(oldValue.value_int() - std::stoi(result[3]));
                    return ret;
                }
                else if (oldValue.Value_case() == cod_pb::Variable::ValueCase::kValueFloat)
                {
                    ret.set_value_int(oldValue.value_float() - std::stof(result[3]));
                    return ret;
                }
                else
                {
                    ret.set_value_int(std::stoi(oldValue.value_string()) - std::stoi(result[3]));
                    return ret;
                }
            }
        }
    }

    {
        // Check if value has no preceding characters, a possible + or - sign
        // and one ore more digits -> its an int
        std::vector<std::string> result = RegexMatch("^\\w*[\\-+]?\\d+", value);
        if (result.size() > 0)
        {
            ret.set_value_int(std::stoi(value));
            return ret;
        }
    }

    {
        // Check if value has no preceding characters, a possible + or - sign and one ore more digits
        // followed by a dot and another one or more digits -> its a float
        std::vector<std::string> result = RegexMatch("^\\w+[\\-+]?\\d+\\.\\d+", value);
        if (result.size() > 0)
        {
            ret.set_value_int(std::stof(value));
            return ret;
        }
    }

    {
        // Check if value contains any other characters besides 0-9, + and -
        // -> it is a pure string
        std::vector<std::string> result = RegexMatch("([A-Za-z0-9_]+)", value);
        if (result.size() > 0)
        {
            // TODO : When is value not in variables
            if (ConstantExists(key) == true)
            {
                auto v = GetVariable(result[1]);
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
int CodParser::ExistsInCurrentObject(const std::string& variableName)
{
    if (currentObject)
    {
        // Check if variable already exists in currentObject (e.g. copied from ObjFill)
        for (int index = 0; index < currentObject->variables().variable_size(); index++)
        {
            if (currentObject->variables().variable(index).name() == variableName)
            {
                return index;
            }
        }
    }
    return -1;
}

cod_pb::Variable* CodParser::CreateOrReuseVariable(const std::string& name)
{
    if (currentObject)
    {
        auto optionalVar = GetVariable(currentObject, name);
        if (optionalVar)
        {
            return optionalVar.value();
        }
    }
    return currentObject->mutable_variables()->add_variable();
}

cod_pb::Variable CodParser::GetVariable(const std::string& key)
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

std::experimental::optional<cod_pb::Variable*> CodParser::GetVariable(cod_pb::Object* obj, const std::string& name)
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

int CodParser::CalculateOperation(int old_value, const std::string& operation, int op)
{
    int currentValue = old_value;
    if (operation == "+")
    {
        currentValue += op;
    }
    else if (operation == "-")
    {
        currentValue -= op;
    }
    else
    {
        currentValue = op;
    }
    return currentValue;
}

// Object stack related functions
bool CodParser::TopIsNumberObject()
{
    if (!objectStack.empty())
    {
        return objectStack.top().numberObject;
    }
    return false;
}

void CodParser::AddToStack(cod_pb::Object* o, bool numberObject, int spaces)
{
    ObjectType obj;
    obj.object = o;
    obj.numberObject = numberObject;
    obj.spaces = spaces;
    objectStack.push(obj);
}

void CodParser::ObjectFinished()
{
    if (objectStack.size() > 0)
    {
        objectStack.pop();
    }
}

CodParser::CodValueType CodParser::CheckType(const std::string& s)
{
    // TODO: replace with boost::regex to get rid of std::regex
    if (std::regex_match(s, std::regex("[-|+]?[0-9]+")))
    {
        return CodValueType::INT;
    }
    else if (std::regex_match(s, std::regex("[-|+]?[0-9]+.[0-9]+")))
    {
        return CodValueType::FLOAT;
    }
    else
    {
        return CodValueType::STRING;
    }
}

void CodParser::DebugOutput()
{
    std::cout << objects.DebugString() << std::endl;
}
