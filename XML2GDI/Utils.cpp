#include "Utils.h"
#include <codecvt>

std::string WStr2Str(const std::wstring& wstr)
{
    static std::wstring_convert<std::codecvt_utf16<wchar_t>> myconv;
    return myconv.to_bytes(wstr);
}

std::wstring Str2WStr(const std::string& str)
{
    static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> myconv;
    return myconv.from_bytes(str);
}

std::vector<std::string> SplitStringByChar(std::string str, char seperator)
{
	std::vector<std::string> result;

	size_t start_offset = 0;
	size_t end_offset = 0;

	while (end_offset != std::string::npos)
	{
		end_offset = str.find(seperator, start_offset);
		result.push_back(str.substr(start_offset, end_offset));
		start_offset = end_offset + 1;
	}

	return result;
}

std::string TrimStr(const std::string& str)
{
    // 去掉首部空格
    auto start = str.find_first_not_of(" \t\n\r\f\v");
    if (start == std::string::npos) {
        return ""; // 全部是空格
    }

    // 去掉尾部空格
    auto end = str.find_last_not_of(" \t\n\r\f\v");

    return str.substr(start, end - start + 1);
}
