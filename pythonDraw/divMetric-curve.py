from pathlib import Path
from utils import ReadAccMetric, PlotLineChart, PlotSeriesLineChart
import pandas as pd
import re
from utils import default_series_config
import os

# datasetList = ["NEUKI2019", "IOTBEHAV2021", "UNSW201620"]

class LabSettings:
    datasetList = ["UNSW201620"]
    methodName = "fixedDiv"
    attribute = "combined"
    graphName = "LineChart"
    start = 15;
    end = 315;
    step = 15;

def getDevList(datasetName):
    path = f"/home/kunling/BurstIoT/mappings/{datasetName}_device_mac_mappings.csv" 
    df = pd.read_csv(path);
    return df['Device'].to_list();

#  attribute: repeat-rate, entropy, diversity, burstRate
def drawSlotDurationAttrCurve(setting, attribute):
    
    for datasetName in setting.datasetList:
        devList = getDevList(datasetName);
        for deviceLabel in devList:
            x_axis =[x for x in range(setting.start, setting.end, setting.step)];
            y_axis = [];
            for fixedSlot in x_axis:
                inPath = Path(f"/media/kunling/BigE/{datasetName}").joinpath(f"fixed-divmetrics-{fixedSlot}.txt");
                y_axis.append(getDivAttribute(inPath, deviceLabel, attribute))

            default_series_config[0]['data'] = y_axis;
            default_series_config[0]['label'] = deviceLabel;

            config = default_series_config[:1] 
            outPath = Path(f"/home/kunling/BurstIoT/pythonDraw/{attribute}/{datasetName}-{deviceLabel}-{setting.methodName}-{setting.attribute}-{setting.graphName}.png");
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


def drawSlotDurationCombinedCurve(setting):
    
    for datasetName in setting.datasetList:
        devList = getDevList(datasetName);
        for deviceLabel in devList:
            x_axis =[x for x in range(setting.start, setting.end, setting.step)];
            y_burstRate = [];
            y_repeatRate = [];
            y_entropy = [];
            
            for fixedSlot in x_axis:
                inPath = Path(f"/media/kunling/BigE/{datasetName}").joinpath(f"fixed-divmetrics-{fixedSlot}.txt");
                y_burstRate.append(getDivAttribute(inPath, deviceLabel, "burstRate"))
                y_repeatRate.append(getDivAttribute(inPath, deviceLabel, "repeat-rate"))
                y_entropy.append(getDivAttribute(inPath, deviceLabel, "entropy"))


            default_series_config[0]['data'] = y_burstRate;
            default_series_config[0]['label'] = "Speed";

            default_series_config[1]['data'] = y_repeatRate;
            default_series_config[1]['label'] = "Repeat Rate";

            default_series_config[2]['data'] = y_entropy;
            default_series_config[2]['label'] = "Entropy";

            config = default_series_config[:3] 
            outPath = Path(f"/home/kunling/BurstIoT/pythonDraw/dev-combined/{datasetName}-{deviceLabel}-{setting.methodName}-{setting.attribute}-{setting.graphName}.png");
            if not Path.is_dir(outPath.parent):
                os.mkdir(outPath.parent)

            yLabel = ""
            PlotSeriesLineChart(x_axis, config, "Slot Duration", yLabel, outPath)

def getDivAttribute(path, deviceLabel, attribute):
    with open(path) as file:
        text = file.read()

        # 第一步：分割文本为独立的设备块
        device_blocks = re.split(r'\n(?=\S+.*traffic division metric:)', text.strip())

        # 第二步：定义提取单个设备信息的正则表达式
        pattern = r'''
            ^(.*?)\s+traffic\s+division\s+metric:  # 设备名称
            \s*repeat-rate:([\d.]+)                # repeat-rate
            \s*entropy:([\d.]+)                    # entropy
            \s*burstRate:([\d.]+)                  # burstRate
            \s*diversity:(\d+)                     # diversity
        '''

        for block in device_blocks:
            match = re.search(pattern, block.strip(), re.VERBOSE | re.DOTALL)
            if not match:
                continue

            device_name = match.group(1).strip()
            repeat_rate = float(match.group(2))
            entropy = float(match.group(3))
            burst_rate = float(match.group(4))
            diversity = int(match.group(5))
                
                
            if(device_name != deviceLabel):
                continue;

            if(attribute == "repeat-rate"):
                return repeat_rate
            elif(attribute == "entropy"):
                return entropy
            elif(attribute == "burstRate"):
                return burst_rate
            elif(attribute == "diversity"):
                return diversity
            else:
                raise "attribute not found!"                   

drawSlotDurationCombinedCurve(LabSettings())