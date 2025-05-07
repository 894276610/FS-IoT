from utils import *
from sklearn.metrics import accuracy_score, precision_score, recall_score, f1_score
import numpy as np
import pandas as pd

def PlotFewShot(labSettings:LabSetting):

    trainBudgetList = np.arange(labSettings.start, labSettings.end, labSettings.step);
    
    byteIoTAccList =  [];
    burstIoTAccList = [];
    shahidAccList = [];
    ahmedAccList = [];

    for trainBudget in trainBudgetList:
        labSettings.configDataset.trainBudget = trainBudget;

        labSettings.methodName = MethodName.FSIOT.name
        resultPath = labSettings.GetPredictionCsvPath();
        print(resultPath)
        burstIoTAccList.append(GetMetric(resultPath)[0]);

        labSettings.methodName = MethodName.BYTEIOT.name
        resultPath = labSettings.GetPredictionCsvPath();
        byteIoTAccList.append(GetMetric(resultPath)[0]);
    
        labSettings.methodName = MethodName.SHAHID.name
        resultPath = labSettings.GetPredictionCsvPath();
        shahidAccList.append(GetMetric(resultPath)[0]);
    
        labSettings.methodName = MethodName.AHMED.name;
        resultPath = labSettings.GetPredictionCsvPath();
        ahmedAccList.append(GetMetric(resultPath)[0]);

    print(trainBudgetList)
    trainingBudgetListHour = [num / 60 for num in trainBudgetList]
    print("burstIoT",burstIoTAccList)
    print("byteIoT", byteIoTAccList)
    print("shahid:", shahidAccList)
    print("ahmed:", ahmedAccList)
    

    default_series_config[0]['data'] = [num * 100 for num in burstIoTAccList];
    default_series_config[0]['label'] = "FS-IoT";

    default_series_config[1]['data'] = [num * 100 for num in byteIoTAccList];
    default_series_config[1]['label'] = "ByteIoT";

    default_series_config[2]['data'] = [num * 100 for num in shahidAccList];
    default_series_config[2]['label'] = "Shahid";

    default_series_config[3]['data'] = [num * 100 for num in ahmedAccList];
    default_series_config[3]['label'] = "Ahmed";

    config = default_series_config[:4] 

    PlotSeriesLineChart(trainingBudgetListHour, config, labSettings.xLabel, labSettings.yLabel, labSettings.GetComparisonGraphPath() , showText=False)

def GetMetric(csv_path):

    # 1. 读取CSV文件
    data = pd.read_csv(csv_path)
    
    # 2. 提取真实标签和预测标签
    y_true = data['y_true']
    y_pred = data['y_pred']
    
    # 3. 计算多分类指标（使用 'macro' 平均）
    accuracy = accuracy_score(y_true, y_pred)
    precision = precision_score(y_true, y_pred, average='macro')  # 宏平均
    recall = recall_score(y_true, y_pred, average='macro')        # 宏平均
    f1 = f1_score(y_true, y_pred, average='macro')               # 宏平均

    return accuracy, precision, recall, f1


def PlotSeriesLineChart(x, series_config, x_axisLabel, y_axisLabel,  outPath, rotation=0, showText = True):

    plt.figure(figsize=(10, 6))

    # 遍历所有数据系列并绘制
    for config in series_config:
        # 合并默认样式与用户自定义样式
        style = {
            'marker': 'o',          # 默认圆形标记
            'linestyle': '-',       # 默认实线
            'linewidth': 2,
            'color': '#2C6FB3',     # 默认蓝色
            'markersize': 8,
            'markeredgecolor': 'black',
            'markerfacecolor': 'white',
            'label': 'Unnamed Series'
        }
        style.update(config)  # 用用户配置覆盖默认值

        # 提取数据并绘制
        y = config['data']
        print(x)
        print(y)
        plt.plot(x, y, 
                 marker=style['marker'],
                 linestyle=style['linestyle'],
                 linewidth=style['linewidth'],
                 color=style['color'],
                 markersize=style['markersize'],
                 markeredgecolor=style['markeredgecolor'],
                 markerfacecolor=style['markerfacecolor'],
                 label=style['label'])
        
        if showText:
            for xi, yi in zip(x, y):
                plt.text(xi, yi+0.01, f'{yi:.2f}',  # 格式化显示两位小数
                    color=style['color'],
                    fontsize=10,
                    ha='center',          # 水平居中
                    va='bottom',          # 垂直底部对齐
                    bbox=dict(facecolor='white', alpha=0.7, edgecolor='none', pad=1))

    # 统一设置标题、标签、网格等
    plt.xlabel(x_axisLabel, fontsize=22)
    plt.ylabel(y_axisLabel, fontsize=22)
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.xticks(x, rotation=rotation, fontsize = 18)
    plt.yticks(fontsize=18)
    plt.legend(fontsize=18)  # 显示图例
    plt.ylim(65, 101)

    plt.tight_layout()

    # 保存图像
    # check if the path has the folder
    import os;
    if not os.path.exists(os.path.dirname(outPath)):
        os.makedirs(os.path.dirname(outPath))

    plt.savefig(outPath, dpi=300, bbox_inches="tight", facecolor="white")
    print(outPath)
    plt.close()

if __name__ == "__main__":
    from settingTemplates import *;
    setting = GetFewShotSettingTemplate();
    PlotFewShot(setting);

# BehavIoT2021-(slot=1800)(trainR=0.15)(trainB=300min)(testR=0.5)(uniTrh=1000)(inTrh=2.0s)(ouTrh=15.0s)(lsenable=1)(Tuni=200)(Tpkt=1000)(mUniPkt=1000)(mPktIdx=500)(dTrh=0.45)(maxTrainIt=1000)(pen=0.01).csv
#               slot=1800)(trainR=0.15)(trainB=60minu)(testR=0.5)(uniTrh=1000)(inTrh=2.0s)(ouTrh=15.0s)(lsenable=1)(Tuni=200)(Tpkt=1000)(mUniPkt=1000)(mPktIdx=500)(dTrh=0.45)(maxTrainIt=1000)(pen=0.01).txt