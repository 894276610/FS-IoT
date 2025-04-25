// #include "labSetting.h"

// #include "boClassifier.h"
// #include "burstDataset.h"
// #include "time-utils.h"
// #include "timer.h"

// std::string GetMetricPath(LabSetting settings);
// std::string GetPredCsvPath(LabSetting settings);
// std::string GetDivisionStem(LabSetting settings, std::string mode);
// timespec Double2time(double ftime);

// void InstancePercentLab(LabSetting settings)
// { 
//     std::string datasetName = settings.datasetName;
//     std::string baseFolder = settings.baseFolder;
//     std::string datasetRootFolder = baseFolder + datasetName + "/";
//     std::string mappingFolder = settings.mappingFolder;
//     std::string pktDatasetSerialName = datasetName + ".pktDataset";

//     groundnut::PacketDataset pktDataset(datasetName);
//     pktDataset.AddTragetDevices( mappingFolder + datasetName + "_device_mac_mappings.csv");
//     pktDataset.AutoLoad(datasetRootFolder, pktDatasetSerialName);
    
//     float& trainRate = settings.config.trainRate;
//     settings.config.trainBudget = 100000000;
//     // for(trainRate = settings.start; trainRate <= settings.end; trainRate += settings.step)
//     // {
//     //     std::cout << "trainRate: " << trainRate << std::endl;

//     //     groundnut::BurstDataset burstDataset(datasetName, settings.config);

//     //     burstDataset.Load(pktDataset);
//     //     burstDataset.TrainTestSplit();
//     //     auto& trainset = burstDataset.GetTrainset();
//     //     auto& testset = burstDataset.GetTestset();
//     //     std::vector<std::string> y_true, y_pred;

//     //     groundnut::BoClassifier boclf(settings.clfConfig);
//     //     boclf.Train(&trainset);

//     //     ResultBundle result;
//     //     groundnut::ReviewBook reviewBook = boclf.Predict(&testset, result, settings.review);
//     //     result.SaveCsv(GetPredCsvPath(settings));
//     // }
// }

// void HourBudgetLab(LabSetting settings)
// {
//     std::string datasetName = settings.datasetName;
//     std::string baseFolder = settings.baseFolder;
//     std::string datasetRootFolder = baseFolder + datasetName + "/";
//     std::string mappingFolder = settings.mappingFolder;
//     std::string pktDatasetSerialName = datasetName + ".pktDataset";

//     groundnut::PacketDataset pktDataset(datasetName);
//     pktDataset.AddTragetDevices( mappingFolder + datasetName + "_device_mac_mappings.csv");
//     pktDataset.AutoLoad(datasetRootFolder, pktDatasetSerialName);

//     int& budget = settings.config.trainBudget;
//     for(budget = settings.start; budget <= settings.end; budget+= settings.step)
//     {
//         Instrumentor::Get().BeginSession("Train", datasetRootFolder + settings.methodName + std::to_string(budget) + "min" + "load-split-train-identification.json");
//         PROFILE_SCOPE("Load-Split-Train-Identification");
//         std::cout << "budget: " << budget << " minute" << std::endl;

//         // ClassificationMetrics metric;
//         groundnut::BurstDataset burstDataset(datasetName, settings.config);

//         burstDataset.Load(pktDataset);
//         burstDataset.TrainTestSplitByTime(budget);

//         std::cout << "before get train and test set" << std::endl;
//         auto& trainset = burstDataset.GetTrainset();
//         auto& testset = burstDataset.GetTestset();
//         std::vector<std::string> y_true, y_pred;
        
//         std::cout << "before clf" << std::endl;
//         groundnut::BoClassifier clf(settings.clfConfig);
//         std::cout << "before train" << std::endl;
//         clf.Train(&trainset);
//         // groundnut::ReviewBook rbook = clf.Predict(&testset, metric, y_true, y_pred, settings.review);
        
//         Instrumentor::Get().EndSession();
//         // SerializePrediction(y_true, y_pred, GetPredCsvPath(settings));
//         // std::ofstream outMetric(GetMetricPath(settings));
//         // outMetric << ToString(metric);
//         // outMetric.close();

//         // rbook.Tofile(GetMetricPath(settings) + ".txt");


//     }
// }

// void DivisionLab(LabSetting settings)
// {
//     std::string datasetName = settings.datasetName;
//     std::string baseFolder = settings.baseFolder;
//     std::string outFolder = baseFolder + datasetName + "/";
//     std::string mappingFolder = settings.mappingFolder;
//     std::string pktDatasetOutName = datasetName + ".pktDataset";

//     groundnut::PacketDataset pktDataset(datasetName);
//     pktDataset.AddTragetDevices( mappingFolder + datasetName + "_device_mac_mappings.csv");
//     pktDataset.AutoLoad(outFolder,pktDatasetOutName);

//     timespec* pVar = nullptr;
//     if(settings.scenario == "ouTrh")
//     {
//         pVar =  &settings.config.burstTrh.ouTrh;
//     }
//     else if(settings.scenario == "inTrh")
//     {
//         pVar = &settings.config.burstTrh.inTrh;
//     }

//     // for(*pVar = Double2time(settings.start); *pVar < Double2time(settings.end); *pVar += settings.step)
//     // {
//     //     groundnut::BurstDataset burstDataset(datasetName, settings.config);
//     //     std::ofstream outMetric(GetDivisionStem(settings, "burst"));
//     //     groundnut::BurstGroups totalSamples;

//     //     burstDataset.Load(pktDataset);
//     //     auto& rawMap = burstDataset.GetRawMap();

//     //     for(auto& [devid, burstGroups]: rawMap)
//     //     {
//     //         std::string deviceName = burstDataset.GetDevicesVec()[devid].GetLabel();
//     //         groundnut::DivMetric metric = burstDataset.GenDivMetric(deviceName, burstGroups);
//     //         totalSamples.insert(totalSamples.end(), burstGroups.begin(), burstGroups.end());
//     //         outMetric << metric.ToString();
//     //     }

//     //     groundnut::DivMetric metric = burstDataset.GenDivMetric("total", totalSamples);
//     //     outMetric << metric.ToString();        
//     //     outMetric.close();
//     // }
// }