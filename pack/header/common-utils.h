#ifndef _GROUNDNUT_COMMON_UTILS_H_
#define _GROUNDNUT_COMMON_UTILS_H_

#include <vector>
#include "types.h"
namespace groundnut{

template<typename T>
std::vector<std::vector<T>> SplitVector(const std::vector<T>& vec, uint32_t chunkSize) {

    std::vector<std::vector<T>> chunks;
    for (size_t i = 0; i < vec.size(); i += chunkSize) {
        // 使用子范围构造函数创建子vector  
        int size = i + chunkSize < vec.size() ? i + chunkSize : vec.size();
        chunks.emplace_back(vec.begin() + i, vec.begin() + size);
    }
    return chunks;
}
}

#endif