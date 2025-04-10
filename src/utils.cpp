#include <string>
#include <sstream>
#include "labSetting.h"

std::string NameDivMetric(LabSetting settings, std::string mode)
{
    std::stringstream ss;
    
    ss << settings.baseFolder + settings.datasetName + "/";
    ss << mode << "-divmetrics";

    if(mode == "fixed")
    {
        ss << "-" << settings.config.slotDuration; 
    }
    else if(mode == "burst")
    {
        ss << settings.config.burstTrh.ToString();
    }

    ss << ".txt";
    return ss.str();
}

std::string GetResultFileName(LabSetting settings)
{
    std::stringstream ss;
    ss << settings.methodName << "-metrics";

    if(settings.methodName == "byteiot")
    {
        ss << "-" << settings.experimentMode << "-" << settings.scenario\
        << settings.config.ToStringWoBurstTrh();
    }
    else if(settings.methodName == "burstiot")
    {
        ss << settings.ToString();
    }
    else if(settings.methodName == "shahid")
    {
        ss << "-" << settings.experimentMode << "-" << settings.scenario\
        << settings.config.ToStringWoBurstTrh() << settings.configShahid.ToString();
    }
    return ss.str();
}

std::string GetMetricPath(LabSetting settings)
{
    std::stringstream ss;
    
    ss << settings.baseFolder + settings.datasetName + "/" \
    << GetResultFileName(settings) << ".txt";

    return ss.str();
}

std::string GetPredCsvPath(LabSetting setting)
{
    std::stringstream ss;
    
    ss << setting.baseFolder + setting.datasetName + "/" \
    << GetResultFileName(setting) << ".csv";

    return ss.str();
}