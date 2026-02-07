#pragma once
#include <chrono>
#include <cstdint>


static inline int64_t NowMs()
{
    using namespace std::chrono;
    return duration_cast<milliseconds>(
        steady_clock::now().time_since_epoch()
    ).count();
}