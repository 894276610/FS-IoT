#include "labSetting.h"

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
    ss << methodName << "-metrics";

    if(methodName == MethodEnum::BYTEIOT)
    {
        ss << "-" << experimentMode << "-" << scenario\
        << config.ToStringWoBurstTrh();
    }
    else if(methodName == MethodEnum::FSIOT)
    {
        ss << ToString();
    }
    else if(methodName == MethodEnum::SHAHID)
    {
        ss << "-" << experimentMode << "-" << scenario\
        << config.ToStringWoBurstTrh() << configShahid.ToString();
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
          ss << "-" << config.slotDuration; 
      }
      else if(mode == "burst")
      {
          ss << config.burstTrh.ToString();
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
    ss << mappingFolder << datasetName << "_device_mac_mappings.csv";
    return ss.str();
}

std::string LabSetting::GetTimeCostFilePath()
{
    std::string budgetStr = std::to_string(config.trainBudget) + "min";
    std::stringstream ss;
    ss << GetDatasetFolder() << methodName << budgetStr << "-timeCost.json";
    return ss.str();
}