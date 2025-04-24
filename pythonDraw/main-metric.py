from classification import *
from utils import *

def GetHourBudgetSettings():
    settings = LabSetting();
    settings.datasetName = "IOTBEHAV2021" #"NEUKI2019"#"IOTBEHAV2021" # #"UNSW201620"
    settings.methodName = "burstiot"; # byteiot
    settings.experimentMode = "hbd"
    settings.scenario = "hbd"
    settings.graphName = "LineChart"

    settings.configDataset.slotDuration = 1800
    settings.configDataset.trainRate = 0.15 # .15
    settings.configDataset.trainBudget = 30
    settings.configDataset.valiRate = 0.3
    settings.configDataset.testRate = 0.5

    settings.configDataset.burstTrh = BurstTrh()
    settings.configDataset.burstTrh.inTrh = 2.0;
    settings.configDataset.burstTrh.ouTrh = 15.0;
    settings.configDataset.burstTrh.uniTrh = 1000;
    settings.configDataset.burstTrh.longShortEnable = 1;

    settings.configBurstClf.distanceTrh = 0.45;

    settings.xLabel = "Traffic length for training (hour)"
    settings.yLabel = "Accuracy(%)"
    settings.start = 30;
    settings.end = 300;
    settings.step = 30;

    if(settings.datasetName == "UNSW201620"):
        settings.cm_width = 12;
        settings.cm_len = 8;
    elif(settings.datasetName == "IOTBEHAV2021"):
        settings.cm_width = 23; #12;
        settings.cm_len = 12 #8;
 
    return settings;

settings = GetHourBudgetSettings();
# GetAhmedMetric(settings)
PlotConfusionMatrix(settings.ResultCsvPath(), outputBaseFolder + "cm-" + settings.datasetName +"-" + settings.ToString() + ".pdf", width=settings.cm_width, len=settings.cm_len)
# PlotBudgetPerformance(settings);