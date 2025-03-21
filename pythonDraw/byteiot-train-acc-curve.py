from pathlib import Path
from utils import ReadAccMetric, PlotLineChart

datasetName = "UNSW201620"
methodName = "byteiot"
graphName = "accLineGraph"

basePath = Path(f"/media/kunling/BigE/{datasetName}");
nameTemplate = "(slotDur=1800)(trainRate={})(testRate=0.5)(uniTrh=50)(inTrh={{2,0}}(ouTrh={{15,0}}byteiot-metrics.txt"
outPath = f"/home/kunling/BurstIoT/pythonDraw/{datasetName}-{methodName}-{graphName}.png";


trainRateList = [x/100 for x in range(1,20,1)];
accList = []

for trainRate in range(1, 20, 1):

    pathStem = nameTemplate.format(trainRate/100);
    path = basePath.joinpath(pathStem);
    
    accList.append(ReadAccMetric(path));

PlotLineChart(trainRateList, accList, outPath)