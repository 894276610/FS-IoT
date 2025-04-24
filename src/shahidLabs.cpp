#include <string>
#include <packetDataset.h>
#include "labSetting.h"
#include "utils-metric.h"
#include "shahidClassifier.h"
#include "shahidDataset.h"
#include "timer.h"

std::string GetMetricPath(LabSetting settings);
std::string GetPredCsvPath(LabSetting settings);
// std::string NameDivMetric(LabSetting settings, std::string mode);

void HourBudgetLab(LabSetting settings)
{
    std::string datasetName = settings.datasetName;
    std::string baseFolder = settings.baseFolder;
    std::string outFolder = baseFolder + datasetName + "/";
    std::string mappingFolder = settings.mappingFolder;
    std::string pktDatasetOutName = datasetName + ".pktDataset";

    groundnut::PacketDataset pktDataset(datasetName);
    pktDataset.AddTragetDevices( mappingFolder + datasetName + "_device_mac_mappings.csv");
    pktDataset.AutoLoad(outFolder,pktDatasetOutName);

    int& budget = settings.config.trainBudget;
    for( budget = settings.start; budget <= settings.end; budget+= settings.step)
    {
        Instrumentor::Get().BeginSession("Train", outFolder + settings.methodName + "load-split-train-identification.json");
        

        std::cout << "budget: " << budget << "min" << std::endl;

        groundnut::ShahidDataset shahidDataset(datasetName, 6, settings.config.slotDuration, 600);
        std::unordered_map<uint16_t, groundnut::ShahidSlots> trainSet, testSet;

        shahidDataset.Load(pktDataset);

        shahidDataset.TrainTestSplit(trainSet, testSet, budget);
        std::vector<std::string> y_true, y_pred;
    
        groundnut::ShahidClassifier clf(shahidDataset.devicesVec);
        clf.Train(&trainSet);
        clf.Predict(&testSet, y_true, y_pred);

        Instrumentor::Get().EndSession();
        
        SerializePrediction(y_true, y_pred, GetPredCsvPath(settings));
    }
}