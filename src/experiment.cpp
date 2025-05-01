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
void AhmedFewShotsExperiment::RunOnce(LabSetting setting)
{
    {
        py::module_ sys = py::module_::import("sys");
        sys.attr("path").attr("append")("/home/kunling/iot-device-fingerprinting-main/src/scripts");
    
        py::module_ trainScript = py::module_::import("ModelTrainingScript"); 
        std::string datasetName = std::string(magic_enum::enum_name(setting.datasetName));
        trainScript.attr("runFewShotOnce")(setting.baseFolder, datasetName, setting.trainBudget);
    }
}

void ShahidFewShotsExperiment::RunOnce(LabSetting setting)
{
    int budget = setting.trainBudget;

    Instrumentor::Get().BeginCsvSession("TimeOverhead", setting.GetTimeOverheadPath());

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

    Instrumentor::Get().EndCsvSession();
    result.SaveCsv(setting.GetPredictionCsvPath());
}
void ByteIoTFewShotsExperiment::RunOnce(LabSetting setting)
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

    Instrumentor::Get().BeginCsvSession("ByteIoT-FewShotsExperiment", setting.GetTimeOverheadPath());
 
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

    Instrumentor::Get().EndCsvSession();
    result.SaveCsv(setting.GetPredictionCsvPath());
}



void FSIoTFewShotsExperiment::RunOnce(LabSetting setting)
{
    int budget = setting.trainBudget;
    std::cout << "Training budget: " << budget << " minute." << std::endl;
    groundnut::ConfigBurstDataset config;

    config.slotDuration = setting.slotDuration;
    config.trainRate = setting.trainRate;
    config.trainBudget = setting.trainBudget;
    config.testRate = setting.testRate;
    config.burstTrh = setting.burstTrh;

    Instrumentor::Get().BeginCsvSession("FSIOT-FewShotsExperiment", setting.GetTimeOverheadPath());

    groundnut::BurstDataset burstDataset(pktDataset.GetName(), config);
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

    Instrumentor::Get().EndCsvSession();
    
    result.SaveCsv(setting.GetPredictionCsvPath());

    if(!setting.clfConfig.review)
    {
        return;
    }

    rbook.Tofile(setting.GetReviewPath());
}

void FSIoTFewShotsExperiment::Preprocessing()
{
    Instrumentor::Get().BeginCsvSession("FSIOT-FewShotsExperiment", setting.GetTimeOverheadPath());
    {
        PROFILE_SCOPE("Preprocessing", setting.ToString());
        pktDataset.UpdateTargetDevices(setting.GetDeviceMappingFilePath());
        pktDataset.AutoLoad(setting.GetRawTrafficFolder(), setting.GetPktDatasetFilePath());
    }
    Instrumentor::Get().EndCsvSession();
}

void ByteIoTFewShotsExperiment::Preprocessing()
{
    Instrumentor::Get().BeginCsvSession("FSIOT-FewShotsExperiment", setting.GetTimeOverheadPath());
    {
        PROFILE_SCOPE("Preprocessing", setting.ToString());
        pktDataset.UpdateTargetDevices(setting.GetDeviceMappingFilePath());
        pktDataset.AutoLoad(setting.GetRawTrafficFolder(), setting.GetPktDatasetFilePath());
    }
    Instrumentor::Get().EndCsvSession();
}

void ShahidFewShotsExperiment::Preprocessing()
{
    Instrumentor::Get().BeginCsvSession("FSIOT-FewShotsExperiment", setting.GetTimeOverheadPath());
    {
        PROFILE_SCOPE("Preprocessing", setting.ToString());
        pktDataset.UpdateTargetDevices(setting.GetDeviceMappingFilePath());
        pktDataset.AutoLoad(setting.GetRawTrafficFolder(), setting.GetPktDatasetFilePath());
    }
    Instrumentor::Get().EndCsvSession();
}

size_t AhmedFewShotsExperiment::CountCsvFiles(const std::filesystem::path& directory)
{
    size_t count = 0;
    
    // 递归遍历目录
    for (const auto& entry : std::filesystem::recursive_directory_iterator(directory)) {
        // 检查是否是常规文件且扩展名为.csv
        if (entry.is_regular_file() && entry.path().extension() == ".csv") {
            ++count;
        }
    }
    
    return count;
}

void AhmedFewShotsExperiment::PcapToCsv()
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

void AhmedFewShotsExperiment::CsvToFeatureData()
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

void AhmedFewShotsExperiment::Preprocessing()
{
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

void FewShotsExperiment::Run()
{
    int& budget = this->setting.trainBudget;
        
    Instrumentor::Get().BeginCsvSession("FSIOT-FewShotsExperiment", setting.GetTimeOverheadPath());
    {
        PROFILE_SCOPE("Preprocessing", setting.ToString());
        Preprocessing();
    }
    Instrumentor::Get().EndCsvSession();

    for(budget = setting.start; budget <= setting.end; budget += setting.step)
    {
        RunOnce(setting);
    }
}