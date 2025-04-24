from pathlib import Path
from utils import  PlotLineChart
from utils_plot import PlotSeriesLineChart
from file_reader import GetDevList, GetDivAttribute
import pandas as pd
import re
from utils import *
import os
import numpy as np

# datasetList = ["NEUKI2019", "IOTBEHAV2021", "UNSW201620"]

def PlotCombined(labSetting):
    
    for datasetName in labSetting.datasetList:
        devList = GetDevList(datasetName);
        devList.append("total")

        for deviceLabel in devList:
            plotHandlers[labSetting.methodName](datasetName, deviceLabel, labSetting);

#  attribute: repeat-rate, entropy, diversity, burstRate
def PlotFixedDivByAttribute(setting):
    attribute = setting.xLabel
    for datasetName in setting.datasetList:
        devList = GetDevList(datasetName);
        for deviceLabel in devList:
            x_axis =[round(x,1) for x in range(setting.start, setting.end, setting.step)];
            y_axis = [];
            for fixedSlot in x_axis:
                inPath = Path(f"/media/kunling/BigE/{datasetName}").joinpath(f"fixed-divmetrics-{fixedSlot}.txt");
                y_axis.append(GetDivAttribute(inPath, deviceLabel, attribute))

            default_series_config[0]['data'] = y_axis;
            default_series_config[0]['label'] = deviceLabel;

            config = default_series_config[:1] 
            outPath = Path(f"/home/kunling/BurstIoT/pythonDraw/{attribute}/{datasetName}-{deviceLabel}-{setting.methodName}-{setting.yLabel}-{setting.graphName}.png");
            if not Path.is_dir(outPath.parent):
                os.mkdir(outPath.parent)

            yLabel = None;
            if attribute == "burstRate":
                yLabel = "Average Rate"
            elif attribute == "entropy":
                yLabel = "Entropy"
            elif attribute == "repeat-rate":
                yLabel = "Repeat Rate"
            elif attribute == "diversity":
                yLabel = "Diversity"
            else:
                yLabel = attribute

            PlotSeriesLineChart(x_axis, config, "Slot Duration", yLabel, outPath)

# x label is ouTrh and draw entropy of two different mode. lsenable = 0/1
def PlotBurstLsEnableByDevice(datasetName, deviceLabel, setting):
    x_axis = np.arange(setting.start, setting.end, setting.step).tolist()
    y_entropy_lsenable, y_entropy_lsdisable = [], [];
    y_repeat_lsenable, y_repeat_lsdisable = [], [];

    xLabel = setting.xLabel;
    assert(xLabel == "ouTrh" or xLabel == "Duration Threshold");

    for var in x_axis: 
        setting.burstTrh.ouTrh = var;

        setting.burstTrh.longShortEnable = 0;   
        disablePath = GetBurstDivPath(datasetName, setting.burstTrh);
        y_entropy_lsdisable.append(GetDivAttribute(disablePath, deviceLabel, "entropy"))
        y_repeat_lsenable.append(GetDivAttribute(disablePath, deviceLabel, "repeat-rate"))

        setting.burstTrh.longShortEnable = 1;
        enablePath = GetBurstDivPath(datasetName, setting.burstTrh);
        y_entropy_lsenable.append(GetDivAttribute(enablePath, deviceLabel, "entropy"))
        y_repeat_lsdisable.append(GetDivAttribute(enablePath, deviceLabel, "repeat-rate"))


    default_series_config[0]['data'] = y_entropy_lsdisable;
    default_series_config[0]['label'] = "Entropy(long short disable)";

    default_series_config[1]['data'] = y_entropy_lsenable;
    default_series_config[1]['label'] = "Entropy(long short enable)";

    default_series_config[2]['data'] = y_repeat_lsdisable;
    default_series_config[2]['label'] = "Repeat Rate(long short disable)";

    default_series_config[3]['data'] = y_repeat_lsenable;
    default_series_config[3]['label'] = "Repeat Rate(long short enable)";

    outPath = Path(f"/home/kunling/BurstIoT/pythonDraw/longShort/{datasetName}-{deviceLabel}-{setting.methodName}-Longshort-combined-{setting.graphName}.png");
    if not Path.is_dir(outPath.parent):
        os.mkdir(outPath.parent)

    yLabel = "Entropy"
    config = default_series_config[:4] 
    PlotSeriesLineChart(x_axis, config, xLabel, yLabel, outPath)

def PlotBurstCombinedByDevice(datasetName, deviceLabel, setting):
    x_axis = np.arange(setting.start, setting.end, setting.step).tolist()
    y_disRate, y_repeatRate, y_burstRate, y_entropy, y_normEntropy, y_uniBurstNum, y_burstNum = [], [], [], [], [], [], [];

    xLabel = setting.xLabel;
    scenario = setting.scenario;

    # special cases
    if(scenario == "lsEnable"):
        PlotBurstLsEnableByDevice(datasetName, deviceLabel, setting);
        return;
    elif(scenario == xLabel):
        pass;
    
    for var in x_axis:           
        if(xLabel == "ouTrh" or xLabel == "Duration Threshold"):
            setting.burstTrh.ouTrh = round(var,1);               
            xLabel = "Duration Threshold";  
        elif(xLabel == "inTrh" or xLabel == "Interval Threshold"):
            setting.burstTrh.inTrh = round(var,1);
            xLabel = "Interval Threshold";
        else:
            raise "independent variable name error:" + xLabel;
        
        inPath = GetBurstDivPath(datasetName, setting.burstTrh);
        y_burstRate.append(GetDivAttribute(inPath, deviceLabel, "burstRate"))
        y_repeatRate.append(GetDivAttribute(inPath, deviceLabel, "repeat-rate"))
        y_entropy.append(GetDivAttribute(inPath, deviceLabel, "entropy"))
        y_normEntropy.append(GetDivAttribute(inPath, deviceLabel, "normEntropy"))
        y_uniBurstNum.append(GetDivAttribute(inPath, deviceLabel, "uniBurstNum"))
        y_burstNum.append(GetDivAttribute(inPath, deviceLabel, "burstNum"))
        y_disRate.append(GetDivAttribute(inPath, deviceLabel, "burstNum")/GetDivAttribute(inPath, deviceLabel, "uniBurstNum"))


    default_series_config[0]['data'] = y_repeatRate;
    default_series_config[0]['label'] = "Repeat Rate";

    default_series_config[1]['data'] = y_entropy;
    default_series_config[1]['label'] = "Entropy";

    default_series_config[2]['data'] = y_burstRate;
    default_series_config[2]['label'] = "Speed";

    default_series_config[3]['data'] = y_normEntropy;
    default_series_config[3]['label'] = "Norm Entropy";

    default_series_config[4]['data'] = y_disRate;
    default_series_config[4]['label'] = "Distinct Rate";

    default_series_config[5]['data'] = y_burstNum;
    default_series_config[5]['label'] = "Burst Num";

    print("y_burstRate length:",len(y_burstRate))
    print("y_repeatRate length:",len(y_repeatRate))
    print("y_entropy length:",len(y_entropy))
    print("y_normEntropy length:",len(y_normEntropy))
    print("y_uniBurstNum length:",len(y_uniBurstNum))
    print("y_burstNum length:",len(y_burstNum))


    config = default_series_config[:5] 

    outPath = Path(f"/home/kunling/BurstIoT/pythonDraw/dev-combined/{datasetName}-{deviceLabel}-{setting.methodName}-{xLabel}-combined-{setting.graphName}.png");
    if not Path.is_dir(outPath.parent):
        os.mkdir(outPath.parent)

    yLabel = ""
    PlotSeriesLineChart(x_axis, config, xLabel, yLabel, outPath)

# byteiot combined
def PlotFixedCombinedByDevice(datasetName, deviceLabel, setting):
    x_axis = np.arange(setting.start, setting.end, setting.step).tolist()
    y_repeatRate, y_burstRate, y_entropy = [], [], [];
    inPath = None;

    for fixedSlot in x_axis:                
        inPath = GetFixedDivPath(datasetName, fixedSlot);
        y_burstRate.append(GetDivAttribute(inPath, deviceLabel, "burstRate"))
        y_repeatRate.append(GetDivAttribute(inPath, deviceLabel, "repeat-rate"))
        y_entropy.append(GetDivAttribute(inPath, deviceLabel, "entropy"))
       
    default_series_config[0]['data'] = y_repeatRate;
    default_series_config[0]['label'] = "Repeat Rate";

    default_series_config[1]['data'] = y_entropy;
    default_series_config[1]['label'] = "Entropy";

    default_series_config[2]['data'] = y_burstRate;
    default_series_config[2]['label'] = "Speed";

    config = default_series_config[:3] 

    outPath = Path(f"/home/kunling/BurstIoT/pythonDraw/dev-combined/{datasetName}-{deviceLabel}-{setting.methodName}-combined-{setting.graphName}.png");
    if not Path.is_dir(outPath.parent):
        os.mkdir(outPath.parent)

    yLabel = ""
    PlotSeriesLineChart(x_axis, config, setting.xLabel, yLabel, outPath)

plotHandlers = {
    "byte": PlotFixedCombinedByDevice,
    "burst": PlotBurstCombinedByDevice,
}

def GetFixedDivPath(datasetName, fixedSlot):
    return Path(f"/media/kunling/BigE/{datasetName}").joinpath(f"fixed-divmetrics-{fixedSlot}.txt");
                
def GetBurstDivPath(datasetName, burstTrh):
    return Path(f"/media/kunling/BigE/{datasetName}/burst-divmetrics(uniTrh={burstTrh.uniTrh})(inTrh={burstTrh.inTrh}s)(ouTrh={burstTrh.ouTrh}s)(lsenable={burstTrh.longShortEnable}).txt")
