// This file is part of the MDCII Game Engine.
// Copyright (C) 2020  Armin Schlegel
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

#include <fstream>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/regex.hpp>
#include <boost/variant.hpp>

#include "cod/codhelpers.hpp"

std::vector<std::string> ReadFile(const std::string& path, bool decode, bool filterEmptyLines)
{
    std::ifstream input(path, std::ios::binary);
    std::vector<std::string> codTxt;
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
            if (filterEmptyLines == false)
            {
                codTxt.push_back(line);
            }
            line = "";
            i++; // hop over '\n'
        }
    }
    return codTxt;
}

std::vector<std::string> ReadFileAsString(const std::string& buffer, bool filterEmptyLines)
{
    std::string line;
    std::vector<std::string> codTxt;
    for (unsigned int i = 0; i < buffer.size() - 1; i++)
    {
        if (buffer[i + 1] != '\n' && buffer[i] != '\r')
        {
            line.append(1, buffer[i]);
        }
        else
        {
            line = TrimCommentFromLine(TabsToSpaces((line)));
            if (filterEmptyLines == false)
            {
                codTxt.push_back(line);
            }
            line = "";
            i++; // hop over '\n'
        }
    }
    return codTxt;
}

// String handling functions
std::vector<std::string> RegexMatch(const std::string& regex, const std::string& str)
{
    std::vector<std::string> ret;
    boost::regex expr{ regex };
    boost::smatch what;
    if (boost::regex_match(str, what, expr))
    {
        for (unsigned int i = 0; i < what.size(); i++)
        {
            ret.push_back(what[i]);
        }
    }
    return ret;
}

std::vector<std::string> RegexSearch(const std::string& regex, const std::string& str)
{
    std::vector<std::string> ret;
    boost::regex expr{ regex };
    boost::smatch what;
    if (boost::regex_search(str, what, expr))
    {
        for (unsigned int i = 0; i < what.size(); i++)
        {
            ret.push_back(what[i]);
        }
    }
    return ret;
}

std::string TabsToSpaces(const std::string& str)
{
    std::string newtext = "  ";
    boost::regex re("\t");

    std::string result = boost::regex_replace(str, re, newtext);
    return result;
}

int CountFrontSpaces(const std::string& str)
{
    int numberOfSpaces = 0;
    std::vector<std::string> result = RegexSearch("(\\s*)(\\w+)", str);
    if (result.size() > 0)
    {
        for (auto& iter : result[1])
        {
            if (iter == ' ')
            {
                numberOfSpaces++;
            }
            if (iter != ' ')
            {
                break;
            }
        }
    }
    return numberOfSpaces;
}

std::string RemoveTrailingCharacters(const std::string& str, const char charToRemove)
{
    auto ret = str;
    ret.erase(ret.find_last_not_of(charToRemove) + 1, std::string::npos);
    return ret;
}

std::string RemoveLeadingCharacters(const std::string& str, const char charToRemove)
{
    auto ret = str;
    ret.erase(0, std::min(ret.find_first_not_of(charToRemove), ret.size() - 1));
    return ret;
}

std::string TrimSpacesLeadingTrailing(const std::string& s)
{
    std::string input = s;
    boost::algorithm::trim(input);
    return input;
}

bool IsEmpty(const std::string& str)
{
    if (str.size() == 0 || std::all_of(str.begin(), str.end(), isspace))
    {
        return true;
    }
    return false;
}

bool IsSubstring(const std::string& str, const std::string& substr)
{
    std::size_t found = str.find(substr);
    if (found != std::string::npos)
    {
        return true;
    }
    return false;
}

std::vector<std::string> SplitByDelimiter(const std::string& str, const std::string& delim)
{
    std::vector<std::string> tokens;
    boost::split(tokens, str, boost::is_any_of(delim));
    return tokens;
}

std::string TrimCommentFromLine(const std::string& str)
{
    return SplitByDelimiter(str, ";")[0];
}

bool BeginsWith(const std::string& str, const std::string& begin)
{
    if (str.rfind(begin, 0) == 0)
    {
        return true;
    }
    return false;
}
