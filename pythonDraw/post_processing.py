from utils import LabSetting
from sklearn.metrics import accuracy_score, precision_score, recall_score, f1_score


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

def postProcessing(labSetting: LabSetting):

    # metric path
    pass