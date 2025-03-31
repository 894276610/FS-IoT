class BurstTrh:
    inTrh = 2.0;
    ouTrh = 4.0;
    uniTrh = 1000;
    longShortEnable = 1;

class LabSettings:
    datasetList = ["UNSW201620"]
    methodName = "burst" # byte
    attribute = "combined"
    graphName = "LineChart"
    burstTrh = BurstTrh()
    start = 4.0;
    end = 60.0;
    step = 3;