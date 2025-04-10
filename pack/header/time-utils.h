#ifndef _GROUNDNUT_TIME_UTILS_H_
#define _GROUNDNUT_TIME_UTILS_H_
#include <ctime>
#include <tuple>
#include <sstream>

inline float Time2Float(const timespec& time)
{
    return static_cast<float>(time.tv_sec) + static_cast<float>(time.tv_nsec) / 1e9f;
}

inline timespec Float2time(float ftime)
{
    long int sec = int(ftime);
    long int nsec = (ftime - sec)*1e9;
    return timespec{sec, nsec};
}

inline std::string ToString(const timespec& time) 
{
    std::stringstream ss;
    ss << time.tv_sec << "." << time.tv_nsec/1e8;
    return ss.str();
}

inline bool operator==(const timespec& lhs, const timespec& rhs) {
    return lhs.tv_sec == rhs.tv_sec && lhs.tv_nsec == rhs.tv_nsec;
}

inline bool operator<(const timespec&lhs, const timespec& rhs){
    return std::tie(lhs.tv_sec, lhs.tv_nsec) < std::tie(rhs.tv_sec, rhs.tv_nsec);
}

inline bool operator>(const timespec&lhs, const timespec& rhs){
    return std::tie(lhs.tv_sec, lhs.tv_nsec) > std::tie(rhs.tv_sec, rhs.tv_nsec);
}

inline timespec operator/(const timespec& lhs, int rhs)
{
    long int sec = lhs.tv_sec / rhs;
    long int secRemainder = lhs.tv_sec % rhs;
    long int nsec = (secRemainder * 1e9 + lhs.tv_nsec)/rhs;
    
    return timespec{sec, nsec};
}

inline float operator/(int lhs, const timespec& rhs)
{
    float rhsTime = rhs.tv_sec + ((float)rhs.tv_nsec)/1e9;
    return lhs/rhsTime;
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

inline timespec operator+(const timespec& a, const timespec& b){
    struct timespec result;

    // 计算秒差和纳秒差
    result.tv_sec = a.tv_sec + b.tv_sec;
    result.tv_nsec = a.tv_nsec + b.tv_nsec;

    // 处理纳秒借位（如果纳秒差为负数）
    if (result.tv_nsec >= 1000000000) {
        result.tv_sec += 1;              // 从秒差借1秒
        result.tv_nsec -= 1000000000;     // 1秒 = 1e9纳秒
    }

    return result;
}

inline timespec& operator+=( timespec& lhs, timespec& rhs) {
    lhs = lhs + rhs;
    return lhs;
}

inline timespec& operator+=( timespec& lhs, float rhs) {
    timespec trhs = Float2time(rhs);
    lhs = lhs + trhs;
    return lhs;
}

#endif