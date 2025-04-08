from classification import *
from utils import *

def GetHourBudgetSettings():
    settings = LabSetting();
    settings.datasetName = "UNSW201620"
    settings.methodName = "burstiot"; # byteiot
    settings.experimentMode = "hbd"
    settings.scenario = "hbd"
    settings.graphName = "LineChart"

    settings.configDataset.slotDuration = 1800
    settings.configDataset.trainRate = 0.15
    settings.configDataset.trainBudget = 30
    settings.configDataset.valiRate = 0.3
    settings.configDataset.testRate = 0.5

    settings.configDataset.burstTrh = BurstTrh()
    settings.configDataset.burstTrh.inTrh = 2.0;
    settings.configDataset.burstTrh.ouTrh = 15.0;
    settings.configDataset.burstTrh.uniTrh = 1000;
    settings.configDataset.burstTrh.longShortEnable = 1;

    settings.xLabel = "Max Training Time (minute)"
    settings.yLabel = "Accuracy"
    settings.start = 30;
    settings.end = 330;
    settings.step = 30;

    return settings;

settings = GetHourBudgetSettings();
PlotConfusionMatrix(settings.ResultCsvPath(), outputBaseFolder + settings.ToString() + ".png")
# PlotBudgetPerformance(settings);
