#ifndef _CODHELPERS_HPP_
#define _CODHELPERS_HPP_

#include <string>
#include <vector>

std::vector<std::string> ReadFile(const std::string& path, bool decode, bool filterEmptyLines = true);
std::vector<std::string> ReadFileAsString(const std::string& buffer, bool filterEmptyLines = true);

// String handling functions
std::vector<std::string> RegexMatch(const std::string& regex, const std::string& str);
std::vector<std::string> RegexSearch(const std::string& regex, const std::string& str);
std::string TabsToSpaces(const std::string& str);
int CountFrontSpaces(const std::string& str);
std::string RemoveTrailingCharacters(const std::string& str, const char charToRemove);
std::string RemoveLeadingCharacters(const std::string& str, const char charToRemove);
std::string TrimSpacesLeadingTrailing(const std::string& s);
bool IsEmpty(const std::string& str);
bool IsSubstring(const std::string& str, const std::string& substr);
std::vector<std::string> SplitByDelimiter(const std::string& str, const std::string& delim);
std::string TrimCommentFromLine(const std::string& str);
bool BeginsWith(const std::string& str, const std::string& begin);
std::string RemoveDigits(const std::string& str);

#endif // _CODHELPERS_HPP_