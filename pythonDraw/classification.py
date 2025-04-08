from pathlib import Path
from utils import *
from utils_plot import *
from file_reader import *
import numpy as np


def PlotBudgetPerformance(labSettings:LabSetting):

    trainBudgetList = np.arange(labSettings.start, labSettings.end, labSettings.step);
    
    byteIoTAccList =  [];
    burstIoTAccList = [];

    for trainBudget in trainBudgetList:
        labSettings.configDataset.trainBudget = trainBudget;

        labSettings.methodName = "burstiot"
        resultPath = labSettings.ResultTxtPath();
        burstIoTAccList.append(ReadAccMetric(resultPath));

        labSettings.methodName = "byteiot"
        resultPath = labSettings.ResultTxtPath();
        byteIoTAccList.append(ReadAccMetric(resultPath));
    

    print(trainBudgetList)
    print(byteIoTAccList)
    PlotLineChart(trainBudgetList, byteIoTAccList, burstIoTAccList, outputBaseFolder +  f"{labSettings.datasetName}" + labSettings.ToString() + ".png")