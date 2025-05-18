#ifndef _LABSETTING_H_
#define _LABSETTING_H_

#include <string>
#include <sstream>
#include <variant>
#include <functional>
#include "burstDataset.h"
#include "burstClassifier.h"
#include "shahidDataset.h"
#include <magic_enum.hpp>

enum class ExperimentEnum {
    FEW_SHOTS,
    DIVISION,
    BURST_EXTRACTION,
    SAME_ACCURACY,
    BURST_INTRH_EVALUATION,
    BYTEIOT_SLOT_LENGTH
};

enum class MethodEnum {
    FSIOT,
    BYTEIOT,
    AHMED,
    SHAHID
};

enum class IndependentArgEnum {
    IN_TRH,
    OUT_TRH,
    TRAINING_SIZE,
    WINDOW_SIZE
};


inline std::ostream& operator<<(std::ostream& os, IndependentArgEnum type) {
    return os << magic_enum::enum_name(type);
}

inline std::ostream& operator<<(std::ostream& os, ExperimentEnum type) {
    return os << magic_enum::enum_name(type);
}

inline std::ostream& operator<<(std::ostream& os, groundnut::DatasetEnum type) {
    return os << magic_enum::enum_name(type);
}

inline std::ostream& operator<<(std::ostream& os, MethodEnum type) {
    return os << magic_enum::enum_name(type);
}

inline std::string operator+(ExperimentEnum lhs, std::string_view rhs) {
    return std::string(magic_enum::enum_name(lhs)) + rhs.data();
}

inline std::string operator+(std::string_view lhs, ExperimentEnum rhs) {
    return lhs.data() + std::string(magic_enum::enum_name(rhs));
}

struct LabSetting
{
    MethodEnum methodName = MethodEnum::FSIOT;
    groundnut::DatasetEnum datasetName = groundnut::DatasetEnum::UNSW201620;
    ExperimentEnum scenario = ExperimentEnum::FEW_SHOTS;

    std::string baseFolder = "/media/kunling/BigE/IoT/";

    int slotDuration = 1800; // 30 minutes
	float trainRate = 0.15f; // Take the more strict limit for training.
	int trainBudget = 10000; // by minute
	float testRate =  0.5f;

    IndependentArgEnum independentArg = IndependentArgEnum::TRAINING_SIZE;
    double start = 0;
    double end = 0;
    double step = 0;

    groundnut::BurstTrh burstTrh;         // FS-IoT setting
    groundnut::FsClfConfig clfConfig;     // FS-IoT classifier setting
    groundnut::ShahidConfig shahidConfig; // Shahid setting

    void* GetIndependentRef();
    void* GetIndependentRef(IndependentArgEnum independentArg);
    std::string GetScenarioInfo();
    std::string GetTrainTestSplitInfo();
    
    std::string GetPythonDrawFolder() const;
    std::string GetRawTrafficFolder();
    std::string GetPktDatasetFilePath();
    std::string GetAhmedFeatureDataPklPath();
    std::string GetAhmedFeatureDevicePklPath();
    std::string GetAhmedDataCSVFolder();

    bool IsAhmedDataCSVFolderEmpty();
    bool IsRawTrafficFolderEmpty();

    std::string GetDeviceMappingFilePath();
    std::string GetTimeOverheadPath();

    std::string GetConfusionMatrixPath();
    std::string GetDivisionCSVPath();
    std::string GetDivisionGraphPath();

    std::string ToString();

    std::string GetComparisonPath();
    std::string GetPredictionCsvPath();
    std::string GetSummaryMetricCsvPath();
    std::string GetReviewPath();

    int GetBGraphWidth();
    int GetBGraphLength();
    int GetCmWidth();
    int GetCmLength();

    size_t CountCsvFiles(const std::filesystem::path& directory);
    size_t CountPcapFiles(const std::filesystem::path& directory);

private:

    std::string GetResultFolder();
    std::string GetDataFolder();
    std::string GetFeatureFolder();
};

LabSetting GetFewShotSettingTemplate();
LabSetting GetDivisionSettingTemplate();

#endif