#include "experiment.h"
#include "timer.h"
#include "boClassifier.h"
#include "byteIoTDataset.h"
#include "byteIoTClassifier.h"
#include "shahidDataset.h"
#include "shahidClassifier.h"
#include "magic_enum.hpp"
#include <pybind11/embed.h>

namespace py = pybind11;
void AhmedExperiment::RunFewShotOnce(LabSetting setting)
{
    {
        py::module_ sys = py::module_::import("sys");
        sys.attr("path").attr("append")("/home/kunling/iot-device-fingerprinting-main/src/scripts");
    
        py::module_ trainScript = py::module_::import("ModelTrainingScript"); 
        std::string datasetName = std::string(magic_enum::enum_name(setting.datasetName));
        trainScript.attr("runFewShotOnce")(setting.baseFolder, datasetName, setting.trainBudget);
    }
}

void ShahidExperiment::RunFewShotOnce(LabSetting setting)
{
    int budget = setting.trainBudget;


    std::cout << "Training budget: " << budget << " minute." << std::endl;

    groundnut::ShahidDataset shahidDataset(pktDataset.GetName(), 6, setting.slotDuration, 600);
    std::unordered_map<uint16_t, groundnut::ShahidSlots> trainSet, testSet;
    groundnut::ShahidClassifier clf(pktDataset.GetDevicesMap());
    ResultBundle result;

    {
        PROFILE_SCOPE("Load", setting.ToString());
        shahidDataset.Load(pktDataset);
    }

    {
        PROFILE_SCOPE("Split", setting.ToString());
        shahidDataset.TrainTestSplit(trainSet, testSet, budget);
    }

    {
        PROFILE_SCOPE("Train", setting.ToString());
        clf.Train(&trainSet);
    }

    {
        PROFILE_SCOPE("Predict", setting.ToString());
        clf.Predict(&testSet, result);
    }

    result.SaveCsv(setting.GetPredictionCsvPath());
}
void ByteIoTExperiment::RunFewShotOnce(LabSetting setting)
{      
    int budget = setting.trainBudget;
    groundnut::ByteIoTClassifier clf;
    groundnut::ConfigDataset config;
    ResultBundle result;

    config.slotDuration = setting.slotDuration;
    config.trainRate = setting.trainRate;
    config.trainBudget = setting.trainBudget;
    config.testRate = setting.testRate;

    std::cout << "Training budget: " << budget << " minute." << std::endl;

 
    groundnut::ByteIoTDataset byteIoTDataset(pktDataset.GetName(), config);

    {
        PROFILE_SCOPE("Load", setting.ToString());
        byteIoTDataset.Load(pktDataset);
    }

    {
        PROFILE_SCOPE("Split", setting.ToString());
        byteIoTDataset.TrainTestSplitByTime(budget);
    }
    
    auto& trainset = byteIoTDataset.GetTrainset();
    auto& testset  = byteIoTDataset.GetTestset();

    {
        PROFILE_SCOPE("Train", setting.ToString());
        clf.Train(&trainset);
    }

    {
        PROFILE_SCOPE("Predict", setting.ToString());
        clf.Predict(&testset, result, setting.clfConfig.review);
    }

    result.SaveCsv(setting.GetPredictionCsvPath());
}

void FSIoTExperiment::RunFewShotOnce(LabSetting setting)
{
    int budget = setting.trainBudget;
    std::cout << "Training budget: " << budget << " minute." << std::endl;
    groundnut::ConfigBurstDataset config;

    config.slotDuration = setting.slotDuration;
    config.trainRate = setting.trainRate;
    config.trainBudget = setting.trainBudget;
    config.testRate = setting.testRate;
    config.burstTrh = setting.burstTrh;
    
    groundnut::BurstDataset burstDataset(this->pktDataset.GetName(), config);
    groundnut::BoClassifier clf(setting.clfConfig);
    ResultBundle result;
    groundnut::ReviewBook rbook;

    {
        PROFILE_SCOPE("Load", setting.ToString());
        burstDataset.Load(pktDataset);
    }

    {
        PROFILE_SCOPE("Split", setting.ToString());
        burstDataset.TrainTestSplitByTime(budget);
    }

    auto& trainset = burstDataset.GetTrainset();
    auto& testset = burstDataset.GetTestset();

    {
        PROFILE_SCOPE("Train", setting.ToString());
        clf.Train(&trainset);    
    }
   
    {
        PROFILE_SCOPE("Predict", setting.ToString());
        rbook = clf.Predict(&testset, result, setting.clfConfig.review);
    }

    
    result.SaveCsv(setting.GetPredictionCsvPath());

    if(!setting.clfConfig.review)
    {
        return;
    }

    rbook.Tofile(setting.GetReviewPath());
}

void FSIoTExperiment::RunDivisionOnce(LabSetting setting)
{
    int budget = setting.trainBudget;
    std::cout << "RunDivisionOnce" << std::endl;
    std::cout << "Training budget: " << budget << " minute." << std::endl;
    groundnut::ConfigBurstDataset config;
    groundnut::DivMetric metric;

    config.slotDuration = setting.slotDuration;
    config.trainRate = setting.trainRate;
    config.trainBudget = setting.trainBudget;
    config.testRate = setting.testRate;
    config.burstTrh = setting.burstTrh;
    
    groundnut::BurstDataset burstDataset(this->pktDataset.GetName(), config);
    groundnut::BurstGroups totalTrainSamples;
    burstDataset.Load(pktDataset);

    burstDataset.TrainTestSplitByTime(budget);
    auto& trainset = burstDataset.GetTrainset();
 
    std::ofstream outDivMetric(setting.GetDivisionCSVPath());
    outDivMetric << metric.ToCsvHeader();

    for(auto& [devid, burstGroups]: trainset)
    {
        std::string deviceName = burstDataset.GetDevicesVec()[devid].GetLabel();
        metric = burstDataset.GenDivMetric(deviceName, burstGroups);
        outDivMetric << metric.ToCsvString();
        totalTrainSamples.insert(totalTrainSamples.end(), burstGroups.begin(), burstGroups.end());
    }

    metric = burstDataset.GenDivMetric("total", totalTrainSamples);
    outDivMetric << metric.ToCsvString();        
    outDivMetric.close();
}

void FSIoTExperiment::RunDivision()
{
    

}

void Experiment::Preprocessing()
{
    pktDataset.UpdateTargetDevices(setting.GetDeviceMappingFilePath());
    pktDataset.AutoLoad(setting.GetRawTrafficFolder(), setting.GetPktDatasetFilePath());
}

void AhmedExperiment::Preprocessing()
{
    PROFILE_SCOPE("Preprocessing", setting.ToString());

    int status = 100;
    std::cout << "Preprocesing..." << std::endl;

    if(std::filesystem::exists(setting.GetAhmedFeatureDataPklPath()))
    {
        status = 3;
    }
    else if(!setting.IsAhmedDataCSVFolderEmpty())
    {
        status = 2;
    }
    else if(!setting.IsRawTrafficFolderEmpty())
    {
        status = 1;
    }

    switch(status)
    {
        case 1:
            std::cout << "Converting pcap files to csv files..." << std::endl;
            PcapToCsv();
        case 2:
            std::cout << "Converting csv files to feature data..." << std::endl;
            CsvToFeatureData();
        case 3:
            std::cout << "Feature data exists, skipping preprocessing..." << std::endl;
            break;
    } 
}

void AhmedExperiment::PcapToCsv()
{
    std::cout << "Converting pcap files to csv files..." << std::endl;
    {

        py::module_ sys = py::module_::import("sys");
        sys.attr("path").attr("append")("/home/kunling/iot-device-fingerprinting-main/src/scripts");
    
        py::module_ pcapToCsvScript = py::module_::import("PCAP2CSV"); 
        std::string datasetName = std::string(magic_enum::enum_name(setting.datasetName));
        pcapToCsvScript.attr("PCAP2CSV")(setting.baseFolder, datasetName);
    }
    std::cout << "Total pcap files: " << setting.CountCsvFiles(setting.GetRawTrafficFolder()) << std::endl;
}

void AhmedExperiment::CsvToFeatureData()
{
    std::cout << "Converting csv files to feature data..." << std::endl;
    {

        py::module_ sys = py::module_::import("sys");
        sys.attr("path").attr("append")("/home/kunling/iot-device-fingerprinting-main/src/scripts");
    
        py::module_ csvToFeatureDataScript = py::module_::import("CSV2FeatureData"); 
      
        std::string datasetName = std::string(magic_enum::enum_name(setting.datasetName));
        std::cout << "About to convert csv files to feature data for dataset: " << std::endl;
        csvToFeatureDataScript.attr("CSV2FeatureDataWrapper")(setting.baseFolder, datasetName);
    }
}

void Experiment::DoFewShot()
{
    int& budget = this->setting.trainBudget;
         
    Instrumentor::Get().BeginCsvSession("Preprocessing", setting.GetTimeOverheadPath());

    Preprocessing();
    
    for(budget = setting.start; budget <= setting.end; budget += setting.step)
    {
        RunFewShotOnce(setting);
        Postprocessing();
    }

    Instrumentor::Get().EndCsvSession();
}

void Experiment::DoDivision()
{

}

// TODO 应该不用依赖具体路径，而是依赖设置中的路径
void Experiment::Postprocessing()
{
    py::module_ sys = py::module_::import("sys");
    std::cout << "Appending path: " << "/home/kunling/IoTClassifier2025/pythonDraw/" << std::endl;
    sys.attr("path").attr("append")("/home/kunling/IoTClassifier2025/pythonDraw/");

    // 提取分类指标
    {
        std::string datasetName, methodName, baseFolder; 
        py::module_ extractScript = py::module_::import("extractMetric"); 
        datasetName = std::string(magic_enum::enum_name(setting.datasetName));
        methodName = std::string(magic_enum::enum_name(setting.methodName));
        baseFolder = setting.baseFolder;
        extractScript.attr("ExtractMetricsWrapper")(baseFolder, methodName, datasetName, setting.GetPredictionCsvPath());
    }

    // 绘制混淆矩阵
    {
        std::string resultCsvPath, CmOutputPath;
        int CmWidth, CmLen;
        resultCsvPath = setting.GetPredictionCsvPath();
        CmOutputPath = setting.GetConfusionMatrixPath();
        CmWidth = setting.GetCmWidth();
        CmLen = setting.GetCmLength();

        py::module_ cmScript = py::module_::import("plotCm");
        cmScript.attr("PlotCmWrapper")(resultCsvPath, CmOutputPath, CmWidth, CmLen);
    }

}