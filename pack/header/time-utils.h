#ifndef _GROUNDNUT_TIME_UTILS_H_
#define _GROUNDNUT_TIME_UTILS_H_
#include <ctime>
#include <tuple>

namespace groundnut{

    inline bool operator==(const timespec& lhs, const timespec& rhs) {
        return lhs.tv_sec == rhs.tv_sec && lhs.tv_nsec == rhs.tv_nsec;
    }
 
    inline bool operator<(const timespec&lhs, const timespec& rhs){
        return std::tie(lhs.tv_sec, lhs.tv_nsec) < std::tie(rhs.tv_sec, rhs.tv_nsec);
    }

    inline timespec operator-(const timespec& a, const timespec& b){
        struct timespec result;

        // 计算秒差和纳秒差
        result.tv_sec = a.tv_sec - b.tv_sec;
        result.tv_nsec = a.tv_nsec - b.tv_nsec;

        // 处理纳秒借位（如果纳秒差为负数）
        if (result.tv_nsec < 0) {
            result.tv_sec -= 1;              // 从秒差借1秒
            result.tv_nsec += 1000000000;     // 1秒 = 1e9纳秒
        }

        return result;
    }
}

#endif