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
class LabelEncoder {
    private:
        std::unordered_map<std::string, int> label_to_index;
        std::vector<std::string> index_to_label;
        bool fitted = false;
    
    public:
        // 训练标签映射关系
        void fit(const std::vector<std::string>& labels) {
            int counter = 0;
            for (const auto& lbl : labels) {
                if (label_to_index.find(lbl) == label_to_index.end()) {
                    label_to_index[lbl] = counter;
                    index_to_label.push_back(lbl);
                    counter++;
                }
            }
            fitted = true;
        }
    
        // 转换字符串标签为整数索引
        std::vector<int> transform(const std::vector<std::string>& labels) const {
            if (!fitted) {
                throw std::runtime_error("LabelEncoder has not been fitted yet");
            }
    
            std::vector<int> encoded;
            for (const auto& lbl : labels) {
                auto it = label_to_index.find(lbl);
                if (it == label_to_index.end()) {
                    throw std::runtime_error("Unknown label encountered: " + lbl);
                }
                encoded.push_back(it->second);
            }
            return encoded;
        }
    
        // 获取类别名称列表
        std::vector<std::string> get_classes() const {
            return index_to_label;
        }
    };

struct ClassificationMetrics {
    std::vector<std::string> classes;
    int num_classes;
    double accuracy;
    
    // 宏观平均（Macro）
    double macro_precision;
    double macro_recall;
    double macro_f1;
    
    // 微观平均（Micro）
    double micro_precision;
    double micro_recall;
    double micro_f1;
    
    std::vector<double> class_precision; // 每个类别的精确率
    std::vector<double> class_recall;    // 每个类别的召回率
    std::vector<double> class_f1;        // 每个类别的F1值
    std::vector<std::vector<int>> confusion_matrix;

};



inline ClassificationMetrics calculate_metrics(
    const std::vector<int>& true_labels,
    const std::vector<int>& pred_labels,
    int num_classes
) {
    ClassificationMetrics metrics;
    metrics.num_classes = num_classes;
    
    // 初始化混淆矩阵 (num_classes x num_classes)
    metrics.confusion_matrix.resize(num_classes, std::vector<int>(num_classes, 0));
    
    // 填充混淆矩阵
    for (size_t i = 0; i < true_labels.size(); ++i) {
        int actual = true_labels[i];
        int predicted = pred_labels[i];
        metrics.confusion_matrix[actual][predicted]++;
    }
    
    // 计算准确率（总正确数 / 样本总数）
    int correct = 0;
    for (int i = 0; i < num_classes; ++i) {
        correct += metrics.confusion_matrix[i][i];
    }
    metrics.accuracy = static_cast<double>(correct) / true_labels.size();
    
    // 计算每个类别的TP, FP, FN
    metrics.class_precision.resize(num_classes, 0.0);
    metrics.class_recall.resize(num_classes, 0.0);
    metrics.class_f1.resize(num_classes, 0.0);
    
    double total_tp = 0, total_fp = 0, total_fn = 0;
    
    for (int cls = 0; cls < num_classes; ++cls) {
        int tp = metrics.confusion_matrix[cls][cls];
        int fp = 0;
        int fn = 0;
        
        for (int other = 0; other < num_classes; ++other) {
            if (other != cls) {
                fp += metrics.confusion_matrix[other][cls]; // 其他类预测为本类的数量
                fn += metrics.confusion_matrix[cls][other]; // 本类预测为其他类的数量
            }
        }
        
        // 计算Precision（若分母为0则置0）
        double precision = (tp + fp == 0) ? 0.0 : static_cast<double>(tp) / (tp + fp);
        metrics.class_precision[cls] = precision;
        
        // 计算Recall（若分母为0则置0）
        double recall = (tp + fn == 0) ? 0.0 : static_cast<double>(tp) / (tp + fn);
        metrics.class_recall[cls] = recall;
        
        // 计算F1（若Precision或Recall为0则置0）
        if (std::isnan(precision) || std::isnan(recall) || (precision + recall == 0)) {
            metrics.class_f1[cls] = 0.0;
        } else {
            metrics.class_f1[cls] = 2 * (precision * recall) / (precision + recall);
        }
        
        // 累加Micro平均所需的全局统计量
        total_tp += tp;
        total_fp += fp;
        total_fn += fn;
    }
    
    // 计算Macro平均
    metrics.macro_precision = 0.0;
    metrics.macro_recall = 0.0;
    metrics.macro_f1 = 0.0;
    
    for (int cls = 0; cls < num_classes; ++cls) {
        metrics.macro_precision += metrics.class_precision[cls];
        metrics.macro_recall += metrics.class_recall[cls];
        metrics.macro_f1 += metrics.class_f1[cls];
    }
    
    metrics.macro_precision /= num_classes;
    metrics.macro_recall /= num_classes;
    metrics.macro_f1 /= num_classes;
    
    // 计算Micro平均
    metrics.micro_precision = (total_tp + total_fp == 0) ? 0.0 : total_tp / (total_tp + total_fp);
    metrics.micro_recall = (total_tp + total_fn == 0) ? 0.0 : total_tp / (total_tp + total_fn);
    
    if (std::isnan(metrics.micro_precision) || std::isnan(metrics.micro_recall) || 
        (metrics.micro_precision + metrics.micro_recall == 0)) {
        metrics.micro_f1 = 0.0;
    } else {
        metrics.micro_f1 = 2 * (metrics.micro_precision * metrics.micro_recall) / 
                          (metrics.micro_precision + metrics.micro_recall);
    }
    
    return metrics;
}

inline ClassificationMetrics calculate_metrics(
    const std::vector<std::string>& true_labels,
    const std::vector<std::string>& pred_labels
) {
    // 步骤1：自动编码字符串标签
    LabelEncoder encoder;
    encoder.fit(true_labels);  // 仅根据真实标签构建映射
    
    try {
        std::vector<int> encoded_true = encoder.transform(true_labels);
        std::vector<int> encoded_pred = encoder.transform(pred_labels);
        
        // 步骤2：计算指标
        int num_classes = encoder.get_classes().size();
        ClassificationMetrics metrics = calculate_metrics(encoded_true, encoded_pred, num_classes);
        
        // 步骤3：保存原始标签信息
        metrics.classes = encoder.get_classes();
        return metrics;
    } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        exit(1);
    }
}

inline std::string GetConfusionMatrix(const ClassificationMetrics& metrics)
{
    std::stringstream ss;

    // with label
    ss << "Confusion Matrix (rows=true, columns=predicted):\n";
    
    // 打印列标签（预测类别）
    ss << "\t\t";
    for (const auto& lbl : metrics.classes) {
        ss << lbl << "\t";
    }
    ss << "\n";
    
    // 打印每行数据
    for (size_t i = 0; i < metrics.classes.size(); ++i) {
        ss << metrics.classes[i] << "\t|\t";
        for (const auto& count : metrics.confusion_matrix[i]) {
            ss << count << "\t";
        }
        ss << "\n";
    } 

    return ss.str();
}

inline std::string ToString(const ClassificationMetrics& metrics) {

    std::stringstream ss;
    
    ss << "Accuracy: " << metrics.accuracy << "\n\n";
    
    ss << "Class-wise Metrics:\n";
    for (int cls = 0; cls < metrics.num_classes; ++cls) {
        ss << "Class " << metrics.classes[cls] << ":\n";
        ss << "  Precision: " << metrics.class_precision[cls] << "\n";
        ss << "  Recall:    " << metrics.class_recall[cls] << "\n";
        ss << "  F1:        " << metrics.class_f1[cls] << "\n\n";
    }
    
    ss << "Macro Average:\n"
              << "  Precision: " << metrics.macro_precision << "\n"
              << "  Recall:    " << metrics.macro_recall << "\n"
              << "  F1:        " << metrics.macro_f1 << "\n\n";
    
    ss << "Micro Average:\n"
              << "  Precision: " << metrics.micro_precision << "\n"
              << "  Recall:    " << metrics.micro_recall << "\n"
              << "  F1:        " << metrics.micro_f1 << "\n\n";
    
    ss << "Confusion Matrix:\n";
    for (const auto& row : metrics.confusion_matrix) {
        for (int val : row) {
            ss << val << "\t";
        }
        ss << "\n";
    }

    ss << GetConfusionMatrix(metrics);

    return ss.str();
}

inline void SerializePrediction(
    const std::vector<std::string>& true_labels,
    const std::vector<std::string>& pred_labels,
    std::filesystem::path outPath)
{
    std::ofstream ofs(outPath);
    ofs << "y_true,y_pred" << std::endl;
    
    for(size_t i = 0; i < true_labels.size(); i ++)
    {
        ofs << true_labels[i] << "," << pred_labels[i] << std::endl;
    }

    ofs.close();
}

#endif