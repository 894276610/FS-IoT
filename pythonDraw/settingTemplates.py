from pythonDraw.commonUtils import *
def GetFewShotSettingTemplate():
    settings = LabSet();
    settings.datasetName = DatasetEnum.BehavIoT2021.name; #"NEUKI2019"#"IOTBEHAV2021" # #"UNSW201620"
    settings.methodName = MethodName.BYTEIOT.name # byteiot
    settings.scenario = ExperimentEnum.FEW_SHOTS.name
    settings.graphName = "LineChart"

    settings.configDataset.slotDuration = 1800
    settings.configDataset.trainRate = 0.15 # .15
    settings.configDataset.trainBudget = 30
    settings.configDataset.testRate = 0.5

    settings.configBurstDataset.slotDuration = 1800
    settings.configBurstDataset.trainRate = 0.15 # .15
    settings.configBurstDataset.trainBudget = 30
    settings.configBurstDataset.testRate = 0.5
    settings.configBurstDataset.burstTrh = BurstTrh()
    settings.configBurstDataset.burstTrh.inTrh = 2.0;
    settings.configBurstDataset.burstTrh.ouTrh = 15.0;
    settings.configBurstDataset.burstTrh.uniTrh = 1000;
    settings.configBurstDataset.burstTrh.longShortEnable = 1;

    settings.configBurstClf.distanceTrh = 0.45;

    settings.xLabel = "Traffic length for training (hour)"
    settings.yLabel = "Accuracy(%)"

    settings.independentArg = IndependentArgEnum.TRAINING_SIZE.name
    settings.start = 30;
    settings.end = 300;
    settings.step = 30;
 
    return settings;
