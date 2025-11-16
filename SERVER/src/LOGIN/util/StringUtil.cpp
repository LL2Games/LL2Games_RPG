#include "util/StringUtil.h"

std::vector<std::string> StringUtil::Split(const char *data, const int len)
{
    std::vector<std::string> out;
    
    int pos = 0;
    while (pos < len)
    {
        std::string s = data + pos;
        out.push_back(s);
        pos += s.size() + 1;
    }

    return out;
}