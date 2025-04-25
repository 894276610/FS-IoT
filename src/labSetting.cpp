#include "labSetting.h"

std::string LabSetting::GetScenarioInfo()
{
    std::stringstream ss;
    ss << methodName << "-" << scenario << "-" ;
    ss << independentArg << "-" << datasetName << "-";
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
std::string LabSetting::GetDatasetFolder()
{
    std::stringstream ss;
    ss << baseFolder << magic_enum::enum_name(datasetName) << "/";
    return ss.str();
}

std::string LabSetting::GetReviewPath()
{
    return GetDatasetFolder() + GetMetricStem() + ".review";
}


std::string LabSetting::GetMetricPath()
{   
    return GetDatasetFolder() + GetMetricStem() + ".txt";
}

std::string LabSetting::GetResultCsvPath()
{
    return GetDatasetFolder() + GetMetricStem() + ".csv";
}


std::string LabSetting::GetMetricStem()
{
    std::stringstream ss;
    ss << GetScenarioInfo() << GetTrainTestSplitInfo();
    switch(methodName)
    {
        case MethodEnum::BYTEIOT:
            break;
        case MethodEnum::FSIOT:
            ss << burstTrh.ToString() << clfConfig.ToString();
            break;
        case MethodEnum::AHMED:
            ss << shahidConfig.ToString();
            break;
    }
    return ss.str();
}
std::string LabSetting::GetDivisionStem()
{
      // 创建一个字符串流
      std::stringstream ss;
      std::string mode;
    
      // 将baseFolder和datasetName拼接，并加上"/"
      ss << GetDatasetFolder();

      if(methodName == MethodEnum::BYTEIOT)
      {
          mode = "fixed";
      }
      else if(methodName == MethodEnum::FSIOT)
      {
          mode = "burst";
      }

      // 将mode和"-divmetrics"拼接
      ss << mode << "-divmetrics";
  
      // 如果mode为"fixed"，则将slotDuration拼接
      if(mode == "fixed")
      {
          ss << "-" << slotDuration; 
      }
      else if(mode == "burst")
      {
          ss << burstTrh.ToString();
      }
  
      ss << ".txt";
      return ss.str();
}

std::string LabSetting::GetRawTrafficFolder()
{
    return GetDatasetFolder() + "RAW/";
}

std::string LabSetting::GetPktDatasetFilePath()
{
    std::stringstream ss;
    ss << GetDatasetFolder() << datasetName << ".pktDataset";
    return ss.str();
}

std::string LabSetting::GetDeviceMappingFilePath()
{
    std::stringstream ss;
    ss << GetDatasetFolder() << "mappings/" << datasetName << "_device_mac_mappings.csv";
    return ss.str();
}

std::string LabSetting::GetTimeCostFilePath()
{
    std::string budgetStr = std::to_string(trainBudget) + "min";
    std::stringstream ss;
    ss << GetDatasetFolder() << methodName << budgetStr << "-timeCost.json";
    return ss.str();
}

LabSetting GetFewShotSettingTemplate()
{
    LabSetting settings;
    settings.baseFolder = "/media/kunling/BigE/";

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
    settings.baseFolder = "/media/kunling/BigE/";

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