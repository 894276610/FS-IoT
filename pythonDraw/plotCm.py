import os;
from pythonDraw.commonUtils import LabSet
import pandas as pd
import numpy as np
from sklearn.metrics import confusion_matrix
import matplotlib.pyplot as plt
import seaborn as sns
from pathlib import Path

def PlotCmWrapper(resultCsvPath, CmOutputPath, CmWidth, CmLen):
    if(not Path.is_dir(Path(CmOutputPath).parent)):
        Path(CmOutputPath).parent.mkdir(parents=True, exist_ok=True)

    PlotConfusionMatrix(resultCsvPath, CmOutputPath, CmWidth, CmLen)

def PlotConfusionMatrix(csv_path, output_image='confusion_matrix.pdf', width = 15, len=10):

    data = pd.read_csv(csv_path)
    
    y_true = data['y_true']
    y_pred = data['y_pred']
    classes = np.unique(np.concatenate([y_true, y_pred]))
    
    cm = confusion_matrix(y_true, y_pred, normalize='true')

    plt.figure(figsize=(width, len))  # 宽度>高度，适应长标签
    
    annot = np.array([[format_value(val) for val in row] for row in cm])

    ax = sns.heatmap(
        cm,
        annot=annot,
        fmt="",
        cmap='Blues',
        xticklabels=classes,
        yticklabels=classes,
        linewidths=0.5,
        square=False,         
        cbar=False
    )
    
    ax.set_xticklabels(ax.get_xticklabels(), rotation=60, ha='right', fontsize=15)
    ax.set_yticklabels(ax.get_yticklabels(), rotation=0, fontsize=15)
    
    plt.xlabel('Predicted Labels', fontsize=20)
    plt.ylabel('True Labels', fontsize=20)
    
    plt.tight_layout()

    # create
    from pathlib import Path

    # if the parent exist


    if(not Path.is_dir(Path(output_image).parent)):
        print(Path(output_image).parent)
        # Path(output_image).parent.mkdir(parents=True, exist_ok=True)

    plt.savefig(output_image, dpi=300, bbox_inches='tight')
    plt.close()

def format_value(val):
        if val == 0:
            return "0"  # 0显示为"0"而非"0.000"
        elif val.is_integer():
            return f"{int(val)}"  # 1.0显示为"1"
        else:
            return f"{val:.3f}".rstrip('0').rstrip('.')  # 0.900→"0.9", 0.123→"0.123"
        
if __name__ == '__main__':
    from settingTemplates import *;
    setting = GetFewShotSettingTemplate();
    setting.methodName = MethodName.FSIOT.name;
    setting.datasetName = DatasetEnum.UNSW201620.name;
    setting.configBurstDataset.trainBudget = 60;
    resultCsvPath = setting.GetPredictionCsvPath();
    CmOutputPath = setting.GetConfusionMatrixPath();
    CmWidth = setting.GetCmWidth();
    CmLen = setting.GetCmLen();
    print(resultCsvPath)
    # resultCsvPath, CmOutputPath, CmWidth, CmLen
    PlotCmWrapper(resultCsvPath, CmOutputPath, CmWidth, CmLen)