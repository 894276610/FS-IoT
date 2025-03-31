import matplotlib.pyplot as plt

# 6条线的配置
default_series_config = [
    {   # 系列1
        'data': None,
        'label': 'ByteIoT',
        'marker': 's',          # 方形标记
        'linestyle': '--',      # 虚线
        'color': '#D95319',     # 橙色
        'markerfacecolor': 'white'
    },
    {   # 系列2
        'data': None,
        'label': 'BurstIoT',
        'marker': 'o',          # 圆形标记
        'color': '#2C6FB3',     # 蓝色
        'markeredgecolor': 'red',
        'markerfacecolor': 'yellow'
    },
    {   # 系列3
        'data': None,
        'label': 'CloudEdge',
        'marker': '^',          # 三角形标记
        'linestyle': '-.',      # 点划线
        'color': '#EDB120',     # 黄色
        'linewidth': 3
    },
    {   # 系列4
        'data': None,
        'label': 'FogNet',
        'marker': 'D',          # 菱形标记
        'color': '#7E2F8E',     # 紫色
        'linestyle': ':',       # 点线
        'markersize': 10
    },
    {   # 系列5
        'data': None,
        'label': 'SmartMesh',
        'marker': 'X',          # 叉形标记
        'color': '#77AC30',     # 绿色
        'alpha': 0.7            # 透明度
    },
    {   # 系列6
        'data': None,
        'label': 'IoTGuard',
        'marker': 'P',          # 五边形标记
        'color': '#A2142F',     # 深红色
        'markeredgecolor': 'black',
        'markerfacecolor': '#A2142F'
    }
]

def ReadAccMetric(filePath):
    with open(filePath, "r+") as file:
        firstLine = file.readline().strip();
        resultList = firstLine.split(" ");
        accFloat = float(resultList[1]);
    
    return accFloat;

def PlotLineChart(x, y, z, outPath):
    
    # 创建画布和坐标轴
    plt.figure(figsize=(10, 6))  # 设置画布大小

    # 绘制折线图
    # ByteIoT
    plt.plot(x, y, 
            marker='s',         # 方形标记
            linestyle='--',     # 虚线
            linewidth=2,
            color='#D95319',    # 橙色
            markersize=8,
            markeredgecolor='#D95319',
            markerfacecolor='white',
            label='ByteIoT')
    
    # BurstIoT
    plt.plot(x, z, 
            marker='o',         # 数据点标记为圆圈
            linestyle='-',      # 实线连接
            linewidth=2,        # 线宽
            color='#2C6FB3',    # 线条颜色（十六进制码）
            markersize=8,       # 标记大小
            markeredgecolor='red',  # 标记边框颜色
            markerfacecolor='yellow',
            label='BurstIoT') # 标记填充色


    
    # 添加标题和标签
    plt.xlabel("Training Rate", fontsize=12)
    plt.ylabel("Accuracy", fontsize=12)

    # 坐标轴美化
    plt.grid(True, linestyle='--', alpha=0.7)  # 显示虚线网格
    plt.xticks(x, rotation=45)                # x轴刻度旋转45度防重叠

    # 显示图表
    plt.tight_layout()  # 自动调整布局
    plt.savefig( outPath,  
    dpi=300,               
    bbox_inches="tight",   
    facecolor="white")
    plt.show()

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


from pathlib import Path

# basePath = Path("/media/kunling/BigE/UNSW201620");
# nameTemplate = "(slotDur=1800)(trainRate=0.01)(uniTrh=50)(inTrh={2,0}(ouTrh={15,0}byteiot-metrics.txt"

# accuracy = ReadAccMetric(basePath.joinpath(nameTemplate))

