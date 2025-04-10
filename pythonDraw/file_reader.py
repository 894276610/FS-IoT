import pandas as pd
import re
import numpy as np
from sklearn.metrics import accuracy_score, precision_score, recall_score, f1_score

def GetDevList(datasetName):
    path = f"/home/kunling/BurstIoT/mappings/{datasetName}_device_mac_mappings.csv" 
    df = pd.read_csv(path);
    return df['Device'].to_list();


def GetMetric(csv_path):

    """
    读取CSV文件，计算准确率
    
    参数:
        csv_path (str): CSV文件路径（格式：第一列是y_true，第二列是y_pred）
    """
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

def ReadAccMetric(filePath):
    with open(filePath, "r+") as file:
        firstLine = file.readline().strip();
        resultList = firstLine.split(" ");
        accFloat = float(resultList[1]);
    
    return accFloat;

def GetDivAttribute(path, deviceLabel, attribute):
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