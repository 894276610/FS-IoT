#ifndef _GROUNDNUT_UTILS_METRIC_H_
#define _GROUNDNUT_UTILS_METRIC_H_

#include <iostream>
#include <vector>
#include <unordered_map>
#include <cmath> 
#include <sstream>
#include <filesystem>
#include <fstream>

// 标签编码器（将字符串映射为唯一整数）
class ResultBundle{

public:

    ResultBundle(): true_labels(), pred_labels() {}
    ResultBundle(std::vector<std::string> true_labels, std::vector<std::string> pred_labels) : true_labels(true_labels), pred_labels(pred_labels) {}
    
    void Update(std::vector<std::string> true_labels, std::vector<std::string> pred_labels)
    {
        this->true_labels = true_labels;
        this->pred_labels = pred_labels;    
    }

    void SaveCsv(const std::filesystem::path& outPath)
    {
        std::ofstream ofs(outPath);
        ofs << "y_true,y_pred" << std::endl;
        
        for(size_t i = 0; i < true_labels.size(); i ++)
        {
            ofs << true_labels[i] << "," << pred_labels[i] << std::endl;
        }
    
        ofs.close();
    }

private:
    std::vector<std::string> true_labels;
    std::vector<std::string> pred_labels;
};

#endif