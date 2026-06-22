#pragma once
#include <vector>
#include <string>

class StringUtil
{
public:
    static std::vector<std::string> Split(const char *data, const int len);
};