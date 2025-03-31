#ifndef _GROUNDNUT_DISTANCE_UTILS_H_
#define _GROUNDNUT_DISTANCE_UTILS_H_

#include <unordered_map>
#include <cmath>

namespace groundnut
{
    float hellinger( std::unordered_map<short,float> umap,  std::unordered_map<short,float> oumap)
    {
        float distance = 0.0f;
        for (const auto& [sig, pct] : umap) {
            if (auto it = oumap.find(sig); it != oumap.end()) {
                distance += pow( sqrt(pct) - sqrt(it->second), 2);
            }else
            {
                distance += pct; 
            }
        }
    
        for(const auto& [sig, pct] : oumap)
        {
            if(auto it = umap.find(sig); it == umap.end())
            {
                distance += pct; 
            }
        } 
        
        return sqrt(distance / 2.0);
    }
}

#endif