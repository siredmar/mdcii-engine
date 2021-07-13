
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

#include <experimental/optional>
#include <stack>
#include <string>
#include <vector>

#include "../cache/cacheprotobuf.hpp"

#include "cod.pb.h"

class CodParser
{
public:
    explicit CodParser(const std::string& codFilePath, bool decode, bool debug);
    explicit CodParser(const std::string& fileAsString);

    void DebugOutput();
    cod_pb::Objects objects;

private:
    // Input/Output related functions
    bool ReadFile(bool decode);
    void ParseFile();
    void Json();

    // Object related functions
    cod_pb::Object* CreateObject(bool numberObject, int spaces, bool addToStack);
    cod_pb::Object* CreateOrReuseObject(const std::string& name, bool numberObject, int spaces, bool addToStack);
    std::experimental::optional<cod_pb::Object*> GetObject(const std::string& name);
    std::experimental::optional<cod_pb::Object*> GetObject(int id);
    std::experimental::optional<cod_pb::Object*> GetSubObject(cod_pb::Object* obj, const std::string& name);
    cod_pb::Object* ObjfillPrefill(cod_pb::Object* obj);
    void ResetObjfillPrefill();

    // Constants related functions
    int ConstantExists(const std::string& key);
    cod_pb::Variable GetValue(const std::string& key, const std::string& value, bool isMath, cod_pb::Variables variables);

    // Variables related functions
    int ExistsInCurrentObject(const std::string& variableName);
    cod_pb::Variable* CreateOrReuseVariable(const std::string& name);
    cod_pb::Variable GetVariable(const std::string& key);
    std::experimental::optional<cod_pb::Variable*> GetVariable(cod_pb::Object* obj, const std::string& name);
    int CalculateOperation(int oldValue, const std::string& operation, int op);

    // Object stack related functions
    bool TopIsNumberObject();
    void AddToStack(cod_pb::Object* o, bool numberObject, int spaces);
    void ObjectFinished();

    struct ObjectType
    {
        cod_pb::Object* object;
        bool numberObject = { false };
        int spaces = -1;
    };

    std::stack<ObjectType> objectStack;

    struct ObjFillRangeType
    {
        cod_pb::Object object;
        std::string start;
        std::string stop;
        unsigned int stacksize = 0;
        bool filling = false;
    };

    ObjFillRangeType objFillRange;

    enum class CodValueType
    {
        INT = 0,
        FLOAT,
        STRING
    };

    CodValueType CheckType(const std::string& s);

    std::string path;
    std::unique_ptr<CacheProtobuf<cod_pb::Objects>> cache;
    std::vector<std::string> codTxt;

    cod_pb::Variables constants;

    std::map<std::string, cod_pb::Object*> objectMap;
    std::map<std::string, int> variableMap;
    std::map<int, cod_pb::Object*> objectIdMap;
    cod_pb::Object* currentObject = nullptr;
    std::vector<std::string> unparsedLines;
};
#endif
