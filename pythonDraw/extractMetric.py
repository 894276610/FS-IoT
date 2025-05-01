# Extract metrics from csv y_true, y_pred file to a csv file with acc, recall, precision, f1, etc.

from utils import *; 
import os;
from pathlib import Path
import pandas as pd
import time
from sklearn.metrics import accuracy_score, precision_score, recall_score, f1_score

# DatasetName, MethodName, Setting, Accuracy, Precision, Recall, F1, Timestamp
def ExtractMetrics(setting:LabSetting):
    
    # output folder init
    outPath = setting.GetSummaryMetricCsvPath();
    if not Path.is_dir(outPath).parent:
        Path(outPath).parent.mkdir(parents=True, exist_ok=True)
   
    data = pd.read_csv(setting.GetPredictionCsvPath())
    y_true = data['y_true']
    y_pred = data['y_pred']
    
    accuracy = accuracy_score(y_true, y_pred)
    precision = precision_score(y_true, y_pred, average='macro')  # 宏平均
    recall = recall_score(y_true, y_pred, average='macro')        # 宏平均
    f1 = f1_score(y_true, y_pred, average='macro')               # 宏平均

    newEntry = pd.DataFrame({
        "MethodName": [setting.methodName],
        "DatasetName": [setting.datasetName],
        "Params": [setting.ToString()], 
        "Accuracy": [accuracy],
        "Precision": [precision],
        "Recall": [recall],
        "F1": [f1],
        "TimeStamp": [time.strftime("%Y-%m-%d %H:%M:%S")],
    })

    if Path(outPath).exists():
        newEntry.to_csv(outPath, mode='a', header=False, index=False)
    else:
        newEntry.to_csv(outPath, mode='w', index=False)
