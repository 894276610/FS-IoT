import matplotlib.pyplot as plt

outputBaseFolder = "/home/kunling/BurstIoT/pythonDraw/classification/"

class BurstTrh:
    inTrh = 2.0;
    ouTrh = 4.0;
    uniTrh = 1000;
    longShortEnable = 1;

    def ToString(self):
        return f"(uniTrh={self.uniTrh})(inTrh={self.inTrh}s)(ouTrh={self.ouTrh}s)(lsenable={self.longShortEnable})";


class ConfigBurstDataset:
    slotDuration = 1800;
    trainRate = 0.15;
    trainBudget = 300;
    valiRate = 0.3;
    testRate = 0.5;
    burstTrh = BurstTrh()
    

    def ToString(self):
        return f"(slot={self.slotDuration})(trainR={self.trainRate}%)(trainB={self.trainBudget}minute)(valiR={self.valiRate}%)(testR={self.testRate}%)" + self.burstTrh.ToString();

    def ToStringWoBurstTrh(self):
        return f"(slot={self.slotDuration})(trainR={self.trainRate}%)(trainB={self.trainBudget}minute)(valiR={self.valiRate}%)(testR={self.testRate}%)";

class ConfigBurstClf:
    def __init__(self, lsenable=True):
        # Initialize with default values based on lsenable
        self.uniPktTolr = 200
        self.pktTolr = 1000
        self.maxUniPkt = 1000  # duplicated with configburstdataset
        self.maxPktIndex = 500
        self.maxTrainItem = 1000
        self.distanceTrh = 0.5
        self.penalty = 0.01
    
    def __str__(self):
        return (f"(Tuni={self.uniPktTolr})"
                f"(Tpkt={self.pktTolr})"
                f"(mUniPkt={self.maxUniPkt})"
                f"(mPktIdx={self.maxPktIndex})"
                f"(dTrh={self.distanceTrh})"
                f"(maxTrainIt={self.maxTrainItem})"
                f"(pen={self.penalty})")

    # Alternative ToString() method if you prefer the original name
    def ToString(self):
        return str(self)
    
class LabSetting:
    baseFolder = "/media/kunling/BigE/";
    datasetName = "UNSW201620";
    mappingFolder = "/home/kunling/BurstIoT/mappings/";
    methodName = "burstiot" # byte
    experimentMode = "hbd"
    scenario = "lsEnable"  
    graphName = "LineChart"

    xLabel = "Slot Duration"
    yLabel = "Accuracy"
    
    configDataset = ConfigBurstDataset()
    configBurstClf = ConfigBurstClf()
    
    start = 4.0;
    end = 60.0;
    step = 3;

    def ResultTxtPath(self):
        return self.baseFolder + self.datasetName + "/" + self.ToString() + ".txt";

    def ResultCsvPath(self):
        return self.baseFolder + self.datasetName + "/" + self.ToString() + ".csv";   
       
    def ToString(self):
        if self.methodName == "burstiot":
            return f"{self.methodName}-metrics-{self.experimentMode}-{self.scenario}{self.configDataset.ToString()}{self.configBurstClf.ToString()}";     
        elif self.methodName == "byteiot":
            return f"{self.methodName}-metrics-{self.experimentMode}-{self.scenario}{self.configDataset.ToStringWoBurstTrh()}";

methodNameList = ["burst", "byte"]

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

from pathlib import Path

# basePath = Path("/media/kunling/BigE/UNSW201620");
# nameTemplate = "(slotDur=1800)(trainRate=0.01)(uniTrh=50)(inTrh={2,0}(ouTrh={15,0}byteiot-metrics.txt"

# accuracy = ReadAccMetric(basePath.joinpath(nameTemplate))

