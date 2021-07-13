#include <algorithm>
#include <codecvt>
#include <locale>
#include <regex>
#include <string>

#include "common/stringhelpers.hpp"

std::wstring stringToWstring(const std::string& utf8Str)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
    return conv.from_bytes(utf8Str);
}

std::string iso_8859_1_to_utf8(std::string& str)
{
    std::string strOut;
    for (std::string::iterator it = str.begin(); it != str.end(); ++it)
    {
        uint8_t ch = *it;
        if (ch < 0x80)
        {
            strOut.push_back(ch);
        }
        else
        {
            strOut.push_back(0xc0 | ch >> 6);
            strOut.push_back(0x80 | (ch & 0x3f));
        }
    }
    return strOut;
}

std::string removeTrailingCarriageReturnNewline(const std::string& input)
{
    std::regex r("[\r\n]{2,}");
    return std::regex_replace(input, r, "\r\n");
}