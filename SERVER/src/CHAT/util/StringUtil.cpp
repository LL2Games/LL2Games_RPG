#include "CHAT/util/StringUtil.h"
#include <algorithm>

std::string StringUtil::Trim(const std::string &s)
{
    auto start = s.find_first_not_of(" \r\n\t");
    auto end = s.find_last_not_of(" \r\n\t");

    if (start == std::string::npos) return "";
    return s.substr(start, end - start + 1);
}