from commonUtils import *
from division import PlotCombined

def GetLsEnableSettings():
    settings = LabSet();
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

def GetInTrhSettings():
    settings = LabSet();
    settings.datasetList = ["UNSW201620"]
    settings.methodName = "burst";
    settings.yLabel = "combined"
    settings.graphName = "LineChart"
    settings.scenario = "inTrh"
    settings.experimentMode = "division"

    settings.burstTrh = BurstTrh();
    settings.burstTrh.inTrh = 2.0;  # should meaningless
    settings.burstTrh.ouTrh = 15.0;        
    settings.burstTrh.uniTrh = 1000;
    settings.burstTrh.longShortEnable = 1; 

    settings.xLabel = "inTrh"
    settings.start = 0.1;
    settings.end = 1.5;
    settings.step = 0.1;

    return settings

# settings = GetLsEnableSettings()
settings = GetInTrhSettings()
PlotCombined(settings)