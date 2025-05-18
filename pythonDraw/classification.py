from pathlib import Path
from commonUtils import *
from utils_plot import *
from file_reader import *
import numpy as np

def GetAhmedMetric(labSettings:LabSet):
    trainBudgetList = np.arange(labSettings.start, labSettings.end, labSettings.step);
    
    for trainBudget in trainBudgetList:
        print("trainBudget:", trainBudget)
        labSettings.methodName = "ahmed"
        resultPath = labSettings.GetResultCsvPath();
        print(GetMetric(resultPath)[0]);


def PlotFewShot(labSettings:LabSet):

    trainBudgetList = np.arange(labSettings.start, labSettings.end, labSettings.step);
    
    byteIoTAccList =  [];
    burstIoTAccList = [];
    shahidAccList = [];
    ahmedAccList = [];

    for trainBudget in trainBudgetList:
        labSettings.configDataset.trainBudget = trainBudget;

        labSettings.methodName = MethodName.FSIOT.name
        resultPath = labSettings.GetResultCsvPath();
        print(resultPath)
        burstIoTAccList.append(GetMetric(resultPath)[0]);

        labSettings.methodName = MethodName.BYTEIOT.name
        resultPath = labSettings.GetResultCsvPath();
        byteIoTAccList.append(GetMetric(resultPath)[0]);
    
        labSettings.methodName = MethodName.SHAHID.name
        resultPath = labSettings.GetResultCsvPath();
        shahidAccList.append(GetMetric(resultPath)[0]);
    
        labSettings.methodName = MethodName.AHMED.name;
        resultPath = labSettings.GetResultCsvPath();
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

    PlotSeriesLineChart(trainingBudgetListHour, config, labSettings.xLabel, labSettings.yLabel, labSettings.GetResultCsvPath() + ".pdf", showText=False)
    # PlotLineChart(trainBudgetList, byteIoTAccList, burstIoTAccList, outputBaseFolder +  f"{labSettings.datasetName}" + labSettings.ToString() + ".png")