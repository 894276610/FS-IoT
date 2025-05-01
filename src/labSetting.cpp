#include "labSetting.h"

bool LabSetting::IsAhmedDataCSVFolderEmpty()
{
    if(!std::filesystem::exists(GetAhmedDataCSVFolder()))
    {
        return true;
    }

    if( CountCsvFiles(GetAhmedDataCSVFolder()) > 0)
    {
        return false;
    }
    else
    {
        return true;
    }
    
}

bool LabSetting::IsRawTrafficFolderEmpty()
{
    if(!std::filesystem::exists(GetRawTrafficFolder()))
    {
        return true;
    }

    if( CountPcapFiles(GetRawTrafficFolder()) > 0)
    {
        return false;
    }
    else
    {
        return true;
    }
}

size_t LabSetting::CountPcapFiles(const std::filesystem::path& directory)
{
    size_t count = 0;
    
    for (const auto& entry : std::filesystem::recursive_directory_iterator(directory)) {
        if (entry.is_regular_file() && (entry.path().extension() == ".pcap" || entry.path().extension() == ".pcapng")) {
            ++count;
        }
    }
    
    return count;
}

size_t LabSetting::CountCsvFiles(const std::filesystem::path& directory)
{
    size_t count = 0;
    
    for (const auto& entry : std::filesystem::recursive_directory_iterator(directory)) {
        if (entry.is_regular_file() && entry.path().extension() == ".csv") {
            ++count;
        }
    }
    
    return count;
}

// 获取场景信息
std::string LabSetting::GetScenarioInfo()
{
    // 创建一个字符串流
    std::stringstream ss;
    // 将方法名、场景、独立参数、数据集名称拼接到字符串流中
    ss << methodName << "-" << scenario << "-" ;
    ss << independentArg << "-" << datasetName << "-";
    // 返回字符串流中的字符串
    return ss.str();
}

std::string LabSetting::GetTrainTestSplitInfo()
{
    std::stringstream ss;
    ss << "(slot=" << slotDuration << ")";
    ss << "(trainR=" << trainRate << ")";
    ss << "(trainB=" << trainBudget << "min)";
    ss << "(testR="<< testRate << ")";
    return ss.str();
}

std::string LabSetting::GetResultFolder()
{
    std::stringstream ss;
    ss << baseFolder << "Result/" <<  magic_enum::enum_name(datasetName) << "/";
    return ss.str();
}

std::string LabSetting::GetFeatureFolder()
{
    std::stringstream ss;
    ss << GetDataFolder() << "Features/";
    return ss.str();
}

std::string LabSetting::GetDataFolder()
{
    std::stringstream ss;
    ss << baseFolder << "Data/" <<  magic_enum::enum_name(datasetName) << "/";
    return ss.str();
}

std::string LabSetting::GetReviewPath()
{
    return GetResultFolder() + "ReviewBook/" + ToString() + ".review";
}


std::string LabSetting::GetPredictionCsvPath()
{
    return GetResultFolder() + "PredictionCSV/" + ToString() + ".csv";
}

std::string LabSetting::ToString()
{
    std::stringstream ss;
    ss << GetScenarioInfo() << GetTrainTestSplitInfo();
    switch(methodName)
    {
        case MethodEnum::AHMED:
            break;
        case MethodEnum::BYTEIOT:
            break;
        case MethodEnum::FSIOT:
            ss << burstTrh.ToString() << clfConfig.ToString();
            break;
        case MethodEnum::SHAHID:
            ss << shahidConfig.ToString();
            break;
    }
    return ss.str();
}

std::string LabSetting::GetAhmedFeatureDataPklPath()
{
    return GetFeatureFolder() + "featureData.pkl";
}
std::string LabSetting::GetAhmedFeatureDevicePklPath()
{
    return GetFeatureFolder() + "devices.pkl";
}

std::string LabSetting::GetAhmedDataCSVFolder()
{
    return GetDataFolder() + "CSV/";
}

std::string LabSetting::GetRawTrafficFolder()
{
    return GetDataFolder() + "Pcap/";
}

std::string LabSetting::GetPktDatasetFilePath()
{
    std::stringstream ss;
    ss << GetFeatureFolder() <<  datasetName << ".pktDataset";
    return ss.str();
}

std::string LabSetting::GetConfusionMatrixPath()
{
    std::stringstream ss;
    ss << GetResultFolder() << "ConfusionMatrix/" << "CM-" << ToString() << ".pdf";
    return ss.str();
}

std::string LabSetting::GetDeviceMappingFilePath()
{
    std::stringstream ss;
    ss << GetDataFolder() << "Map/" << datasetName << "_device_mac_mappings.csv";
    return ss.str();
}

std::string LabSetting::GetTimeOverheadPath()
{
    std::stringstream ss;
    ss << GetResultFolder() << "/TimeOverhead/" << datasetName << "-" << methodName << "-TimeOverhead.csv";
    return ss.str();
}

LabSetting GetFewShotSettingTemplate()
{
    LabSetting settings;
    settings.baseFolder = "/media/kunling/BigE/IoT/";

    settings.methodName = MethodEnum::FSIOT;
    settings.datasetName = groundnut::DatasetEnum::UNSW201620; // "NEUKI2019"; //"UNSW201620"; //"NEUKI2019" //IOTBEHAV2021
    settings.independentArg = IndependentArgEnum::TRAINING_SIZE;
    settings.scenario = ExperimentEnum::FEW_SHOTS;
    settings.burstTrh.inTrh = {2,0};
    settings.start = 30; // sec slotduration
    settings.end = 30;
    settings.step = 30;
    settings.clfConfig.review = false;
    return settings;
}

LabSetting GetDivisionSettingTemplate()
{
    LabSetting settings;
    settings.baseFolder = "/media/kunling/BigE/IoT/";
    settings.methodName = MethodEnum::FSIOT;
    settings.datasetName = groundnut::DatasetEnum::UNSW201620; 
    settings.scenario = ExperimentEnum::DIVISION;

    settings.independentArg = IndependentArgEnum::IN_TRH;
    //settings.config;
    settings.burstTrh.inTrh = {2,0};
    settings.start = 0.1; // sec slotduration
    settings.end = 0.5;
    settings.step = 0.1;
    settings.clfConfig.review = false;
    return settings;
}

// std::string LabSetting::GetDivisionStem()
// {
//       // 创建一个字符串流
//       std::stringstream ss;
//       std::string mode;
    
//       // 将baseFolder和datasetName拼接，并加上"/"
//       ss << GetResultFolder();

//       if(methodName == MethodEnum::BYTEIOT)
//       {
//           mode = "fixed";
//       }
//       else if(methodName == MethodEnum::FSIOT)
//       {
//           mode = "burst";
//       }

//       // 将mode和"-divmetrics"拼接
//       ss << mode << "-divmetrics";
  
//       // 如果mode为"fixed"，则将slotDuration拼接
//       if(mode == "fixed")
//       {
//           ss << "-" << slotDuration; 
//       }
//       else if(mode == "burst")
//       {
//           ss << burstTrh.ToString();
//       }
  
//       ss << ".txt";
//       return ss.str();
// }