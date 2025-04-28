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

std::string LabSetting::GetResultFolder()
{
    std::stringstream ss;
    ss << baseFolder << "Result/" <<  magic_enum::enum_name(datasetName) << "/";
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
// 

std::string LabSetting::GetRawTrafficFolder()
{
    return GetDataFolder() + "Pcap/";
}

std::string LabSetting::GetPktDatasetFilePath()
{
    std::stringstream ss;
    ss << GetDataFolder() << "Features/" <<  datasetName << ".pktDataset";
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