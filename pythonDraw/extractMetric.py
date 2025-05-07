# Extract metrics from csv y_true, y_pred file to a csv file with acc, recall, precision, f1, etc.

from utils import *; 
import os;
from pathlib import Path
import pandas as pd
from settingTemplates import *
import time
from sklearn.metrics import accuracy_score, precision_score, recall_score, f1_score

# DatasetName, MethodName, Setting, Accuracy, Precision, Recall, F1, Timestamp
def ExtractMetrics(setting:LabSetting, predictionCsvPath=None):
    
    # output folder init
    outPath = setting.GetSummaryMetricCsvPath();
    if not Path.is_dir(Path(outPath).parent):
        Path(outPath).parent.mkdir(parents=True, exist_ok=True)
   
    data = pd.read_csv(predictionCsvPath)
    y_true = data['y_true']
    y_pred = data['y_pred']
    
    accuracy = accuracy_score(y_true, y_pred)
    precision = precision_score(y_true, y_pred, average='macro')  # 宏平均
    recall = recall_score(y_true, y_pred, average='macro')        # 宏平均
    f1 = f1_score(y_true, y_pred, average='macro')               # 宏平均

    newEntry = pd.DataFrame({
        "MethodName": [setting.methodName],
        "DatasetName": [setting.datasetName],
        "Params": [predictionCsvPath], 
        "Accuracy": [accuracy],
        "Precision": [precision],
        "Recall": [recall],
        "F1": [f1],
        "TimeStamp": [time.strftime("%Y-%m-%d %H:%M:%S")],
    })

    print("output the metrics");
    print("outpath",  outPath)
    print("newEntry", newEntry)

    if Path(outPath).exists():
        newEntry.to_csv(outPath, mode='a', header=False, index=False)
    else:
        newEntry.to_csv(outPath, mode='w', index=False)

def ExtractMetricsWrapper(baseFolder, methodName, datasetName, csv_path):

    setting = GetFewShotSettingTemplate();
    setting.baseFolder = baseFolder;
    setting.datasetName = datasetName;
    setting.methodName = methodName;
    ExtractMetrics(setting, csv_path);

if __name__ == "__main__":
    ExtractMetricsWrapper("/media/kunling/dpan/IoT/", "UNSW201620", "FSIOT", "/media/kunling/BigE/IoT/Result/UNSW201620/PredictionCSV/FSIOT-FEW_SHOTS-TRAINING_SIZE-UNSW201620-(slot=1800)(trainR=0.15)(trainB=300min)(testR=0.5)(uniTrh=1000)(inTrh=2.0s)(ouTrh=15.0s)(lsenable=1)(Tuni=200)(Tpkt=1000)(mUniPkt=1000)(mPktIdx=500)(dTrh=0.45)(maxTrainIt=1000)(pen=0.01).csv")