#ifndef __STRINGHELPERS_HPP_
#define __STRINGHELPERS_HPP_

std::wstring stringToWstring(const std::string& utf8Str);
std::string iso_8859_1_to_utf8(std::string& str);
std::string removeTrailingCarriageReturnNewline(const std::string& input);

#endif // __STRINGHELPERS_HPP_