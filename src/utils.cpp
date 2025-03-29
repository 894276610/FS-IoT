#include <string>
#include <sstream>
#include "labSetting.h"

std::string NameClassifyMetric(LabSetting settings, std::string methodName)
{
    std::stringstream ss;
    
    ss << settings.baseFolder + settings.datasetName + "/";
    ss << methodName << "-metrics";

    if(methodName == "byteiot")
    {
        ss << "-" << settings.experimentMode 
        << settings.config.ToString();
    }
    else if(methodName == "burstiot")
    {
        ss << settings.ToString();
    }
    
    ss << ".txt";
    return ss.str();
}