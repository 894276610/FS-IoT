import matplotlib.pyplot as plt
from sklearn.metrics import confusion_matrix, ConfusionMatrixDisplay
import pandas as pd
import numpy as np
import seaborn as sns

def format_value(val):
        if val == 0:
            return "0"  # 0显示为"0"而非"0.000"
        elif val.is_integer():
            return f"{int(val)}"  # 1.0显示为"1"
        else:
            return f"{val:.3f}".rstrip('0').rstrip('.')  # 0.900→"0.9", 0.123→"0.123"
        
def PlotConfusionMatrix(csv_path, output_image='confusion_matrix.png'):
    """
    读取CSV文件，绘制混淆矩阵，并保存为图片
    
    参数:
        csv_path (str): CSV文件路径（格式：第一列是y_true，第二列是y_pred）
        output_image (str): 保存的图片文件名（默认：confusion_matrix.png）
    """
    # 1. 读取CSV文件
    data = pd.read_csv(csv_path)
    
    # 2. 提取真实标签和预测标签
    y_true = data['y_true']
    y_pred = data['y_pred']
    classes = np.unique(np.concatenate([y_true, y_pred]))
    
    # 3. 计算混淆矩阵
    cm = confusion_matrix(y_true, y_pred, normalize='true')
    #cm = confusion_matrix(y_true, y_pred)

       # 4. 创建画布（可自定义大小，非正方形）
    plt.figure(figsize=(11, 8))  # 宽度>高度，适应长标签
    
    annot = np.array([[format_value(val) for val in row] for row in cm])

    # 5. 绘制热力图（控制小数位数和样式）
    ax = sns.heatmap(
        cm,
        annot=annot,
        fmt="",
        cmap='Blues',
        xticklabels=classes,
        yticklabels=classes,
        linewidths=0.5,
        square=False,         # 使每个单元格为正方形
        cbar=False
    )
    
    # 6. 旋转x轴标签（避免重叠）
    ax.set_xticklabels(ax.get_xticklabels(), rotation=60, ha='right')
    ax.set_yticklabels(ax.get_yticklabels(), rotation=0)
    
    # 7. 添加标签和标题
    plt.xlabel('Predicted Labels', fontsize=20)
    plt.ylabel('True Labels', fontsize=20)
    
    # 8. 调整布局并保存
    plt.tight_layout()
    plt.savefig(output_image, dpi=300, bbox_inches='tight')
    plt.close()

def PlotSeriesLineChart(x, series_config, x_axisLabel, y_axisLabel, outPath):
    """
    绘制多数据系列的折线图

    参数:
    - x: 共享的横坐标数据（列表或数组）
    - series_config: 包含多个数据系列配置的列表，每个配置是字典，例如:
      [{'data': y1, 'label': 'Series1', 'color': 'red', 'marker': 'o'},
       {'data': y2, 'linestyle': '--', 'linewidth': 3}]
    - outPath: 输出图片路径
    """
    
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
        plt.plot(x, y, 
                 marker=style['marker'],
                 linestyle=style['linestyle'],
                 linewidth=style['linewidth'],
                 color=style['color'],
                 markersize=style['markersize'],
                 markeredgecolor=style['markeredgecolor'],
                 markerfacecolor=style['markerfacecolor'],
                 label=style['label'])
        
        for xi, yi in zip(x, y):
            plt.text(xi, yi+0.01, f'{yi:.2f}',  # 格式化显示两位小数
                color=style['color'],
                fontsize=10,
                ha='center',          # 水平居中
                va='bottom',          # 垂直底部对齐
                bbox=dict(facecolor='white', alpha=0.7, edgecolor='none', pad=1))

    # 统一设置标题、标签、网格等
    plt.xlabel(x_axisLabel, fontsize=12)
    plt.ylabel(y_axisLabel, fontsize=12)
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.xticks(x, rotation=45)
    plt.legend()  # 显示图例

    plt.tight_layout()
    plt.savefig(outPath, dpi=300, bbox_inches="tight", facecolor="white")
    plt.close()