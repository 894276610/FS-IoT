from pathlib import Path
from utils import *
from utils_plot import *
from file_reader import *
import numpy as np


def PlotBudgetPerformance(labSettings:LabSetting):

    trainBudgetList = np.arange(labSettings.start, labSettings.end, labSettings.step);
    
    byteIoTAccList =  [];
    burstIoTAccList = [];
    shahidAccList = [];

    for trainBudget in trainBudgetList:
        labSettings.configDataset.trainBudget = trainBudget;

        labSettings.methodName = "burstiot"
        resultPath = labSettings.ResultTxtPath();
        burstIoTAccList.append(ReadAccMetric(resultPath));

        labSettings.methodName = "byteiot"
        resultPath = labSettings.ResultTxtPath();
        byteIoTAccList.append(ReadAccMetric(resultPath));
    
        labSettings.methodName = "shahid"
        resultPath = labSettings.ResultCsvPath();
        shahidAccList.append(GetMetric(resultPath)[0]);
    

    print(trainBudgetList)
    print("burstIoT",burstIoTAccList)
    print("byteIoT", byteIoTAccList)
    print("shahid:", shahidAccList)
    return
    PlotLineChart(trainBudgetList, byteIoTAccList, burstIoTAccList, outputBaseFolder +  f"{labSettings.datasetName}" + labSettings.ToString() + ".png")