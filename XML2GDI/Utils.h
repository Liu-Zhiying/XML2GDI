#ifndef UTILS_H

#include <string>
#include <vector>

std::string WStr2Str(const std::wstring& wstr);
std::wstring Str2WStr(const std::string& str);

#define ARRAY_SIZE(arr) (sizeof((arr)) / sizeof((arr)[0]))

std::vector<std::string> SplitStringByChar(std::string str, char seperator);

std::string TrimStr(const std::string& str);

#endif // !UTILS_H

