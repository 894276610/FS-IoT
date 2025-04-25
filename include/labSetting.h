#ifndef _LABSETTING_H_
#define _LABSETTING_H_

#include <string>
#include <sstream>
#include "burstDataset.h"
#include "burstClassifier.h"
#include "shahidDataset.h"
#include <magic_enum.hpp>

enum class ExperimentEnum {
    FEW_SHOTS,
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
    std::string baseFolder = "/media/kunling/BigE/";
    MethodEnum methodName = MethodEnum::FSIOT;
    groundnut::DatasetEnum datasetName = groundnut::DatasetEnum::UNSW201620;
    std::string mappingFolder = "/home/kunling/BurstIoT/mappings/";
    std::string experimentMode = "ipc";
    ExperimentEnum scenario;

    groundnut::ConfigBurstDataset config;
    groundnut::ConfigBurstClf clfConfig;
    groundnut::ConfigShahid configShahid;

    double start = 0.01;
    double end = 0.15;
    double step = 0.01;
    bool review = false;

    std::string ToString()
    {
        std::stringstream ss;
        ss << "-" << experimentMode << "-" << scenario << config.ToString() << clfConfig.ToString();
        return ss.str();
    }

    std::string GetRawTrafficFolder();
    std::string GetPktDatasetFilePath();
    std::string GetDeviceMappingFilePath();
    std::string GetTimeCostFilePath();

    std::string GetMetricStem();
    std::string GetMetricPath();

    std::string GetDivisionStem();
    std::string GetDivisionPath();

    std::string GetResultCsvPath();
    std::string GetReviewPath();

private:
    std::string GetDatasetFolder();

};

#endif