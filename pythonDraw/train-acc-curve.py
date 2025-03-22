from pathlib import Path
from utils import ReadAccMetric, PlotLineChart


# datasetList = ["NEUKI2019", "IOTBEHAV2021", "UNSW201620"]
datasetList = ["UNSW201620"]

def drawTrainAccCurve(datasetList):

    for datasetName in datasetList:
        methodName = "burstIoT-byteIoT-0322-1032"
        graphName = "accLineGraph"

        basePath = Path(f"/media/kunling/BigE/{datasetName}");
        byteIoTNameTemplate = "(slotDur=1800)(trainRate={})(valiRate=0.3)(testRate=0.5)(uniTrh=50)(inTrh={{2,0}}(ouTrh={{15,0}}byteiot-metrics.txt"
        burstIoTNameTemplate = "(uniPkTolr=10)(pkTolr=100)(maxUniPkt=50)(maxPktIndex=500)(distanceTrh=0.5)(maxTrainIt=1000)(penalty=0.01)(slotDur=1800)(trainRate={})(testRate=0.5)(uniTrh=50)(inTrh={{2,0}}(ouTrh={{15,0}}metrics.txt"

        outPath = f"/home/kunling/BurstIoT/pythonDraw/{datasetName}-{methodName}-{graphName}.png";

        rangeList = range(1, 16, 1);
        trainRateList = [x/100 for x in rangeList];
        byteIoTAccList = [];
        burstIoTAccList = [];
        bigDataAccList = [0.2]*15;

        for trainRate in rangeList:

            pathStem = burstIoTNameTemplate.format(trainRate/100);
            path = basePath.joinpath(pathStem);
            burstIoTAccList.append(ReadAccMetric(path));

            pathStem = byteIoTNameTemplate.format(trainRate/100);
            path = basePath.joinpath(pathStem);
            byteIoTAccList.append(ReadAccMetric(path));
        

        PlotLineChart(trainRateList, byteIoTAccList, burstIoTAccList, outPath)

drawTrainAccCurve(datasetList);