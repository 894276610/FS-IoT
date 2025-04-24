from pathlib import Path
from utils import *
from utils_plot import *
from file_reader import *
import numpy as np

def GetAhmedMetric(labSettings:LabSetting):
    trainBudgetList = np.arange(labSettings.start, labSettings.end, labSettings.step);
    
    for trainBudget in trainBudgetList:
        print("trainBudget:", trainBudget)
        labSettings.methodName = "ahmed"
        resultPath = labSettings.ResultCsvPath();
        print(GetMetric(resultPath)[0]);


def PlotBudgetPerformance(labSettings:LabSetting):

    trainBudgetList = np.arange(labSettings.start, labSettings.end, labSettings.step);
    
    byteIoTAccList =  [];
    burstIoTAccList = [];
    shahidAccList = [];
    ahmedAccList = [];

    for trainBudget in trainBudgetList:
        labSettings.configDataset.trainBudget = trainBudget;

        labSettings.methodName = "burstiot"
        resultPath = labSettings.ResultTxtPath();
        print(resultPath)
        burstIoTAccList.append(ReadAccMetric(resultPath));

        labSettings.methodName = "byteiot"
        resultPath = labSettings.ResultTxtPath();
        byteIoTAccList.append(ReadAccMetric(resultPath));
    
        labSettings.methodName = "shahid"
        resultPath = labSettings.ResultCsvPath();
        shahidAccList.append(GetMetric(resultPath)[0]);
    
        labSettings.methodName = "ahmed"
        resultPath = labSettings.ResultCsvPath();
        ahmedAccList.append(GetMetric(resultPath)[0]);

    print(trainBudgetList)
    trainingBudgetListHour = [num / 60 for num in trainBudgetList]
    print("burstIoT",burstIoTAccList)
    print("byteIoT", byteIoTAccList)
    print("shahid:", shahidAccList)
    print("ahmed:", ahmedAccList)
    

    default_series_config[0]['data'] = [num * 100 for num in burstIoTAccList];
    default_series_config[0]['label'] = "FS-IoT";

    default_series_config[1]['data'] = [num * 100 for num in byteIoTAccList];
    default_series_config[1]['label'] = "ByteIoT";

    default_series_config[2]['data'] = [num * 100 for num in shahidAccList];
    default_series_config[2]['label'] = "Shahid";

    default_series_config[3]['data'] = [num * 100 for num in ahmedAccList];
    default_series_config[3]['label'] = "Ahmed";

    config = default_series_config[:4] 

    PlotSeriesLineChart(trainingBudgetListHour, config, labSettings.xLabel, labSettings.yLabel, outputBaseFolder +  f"{labSettings.datasetName}" + labSettings.ToString() + ".pdf", showText=False)
    # PlotLineChart(trainBudgetList, byteIoTAccList, burstIoTAccList, outputBaseFolder +  f"{labSettings.datasetName}" + labSettings.ToString() + ".png")