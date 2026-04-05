#include "utility.h"


namespace utility
{
    inline bool StringToInt(std::string_view s, int& output)
    {
        const char* b = s.data();
        const char* e = s.data() + s.size();

        auto [p, ec] = std::from_chars(b, e, output);
        return ec == std::errc{} && p == e; // 끝까지 다 읽었으면 성공
    };

    inline bool StringToFloat(const std::string& s, float& out)
    {
        char* end = nullptr;
        errno = 0;
        out = std::strtof(s.c_str(), &end);
        if (errno != 0) return false;                // 범위 오류 등
        if (end == s.c_str()) return false;          // 변환된 게 없음
        return *end == '\0';                         // 끝까지 다 소비했는지
    };
}

