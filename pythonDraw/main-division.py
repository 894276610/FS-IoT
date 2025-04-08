from utils import *
from division import PlotCombined

def GetLsEnableSettings():
    settings = LabSetting();
    settings.datasetList = ["UNSW201620"]
    settings.methodName = "burst";
    settings.yLabel = "combined"
    settings.graphName = "LineChart"
    settings.scenario = "lsEnable"
    settings.experimentMode = "division"

    settings.burstTrh = BurstTrh()
    settings.burstTrh.inTrh = 2.0;
    settings.burstTrh.ouTrh = 4.0;         # should meaningless
    settings.burstTrh.uniTrh = 1000;
    settings.burstTrh.longShortEnable = 1; # should meaningless

    settings.xLabel = "ouTrh"
    settings.start = 5.0;
    settings.end = 20.0;
    settings.step = 1;

    return settings

settings = GetLsEnableSettings()
PlotCombined(settings)