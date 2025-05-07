import matplotlib.pyplot as plt
from enum import Enum

class MethodName(Enum):
    FSIOT = 1;
    BYTEIOT = 2;
    AHMED = 3;
    SHAHID = 4;

class ExperimentEnum(Enum):
    FEW_SHOTS = 1;
    DIVISION = 2;
    SAME_ACCURACY = 3;

class IndependentArgEnum(Enum):
    IN_TRH = 1;
    OUT_TRH = 2;
    TRAINING_SIZE = 3;
    WINDOW_SIZE = 4;

class DatasetEnum(Enum):
    UNSW201620 = 1;
    BehavIoT2021 = 2;

class ConfigShahid:
    configN = 6;
    timeout = 600;

    def ToString(self):
        return f"(configN={self.configN})(timeout={self.timeout})";
    
class BurstTrh:
    inTrh = 2.0;
    ouTrh = 4.0;
    uniTrh = 1000;
    longShortEnable = 1;

    def ToString(self):
        return f"(uniTrh={self.uniTrh})(inTrh={self.inTrh}s)(ouTrh={self.ouTrh}s)(lsenable={self.longShortEnable})";

class ConfigDataset:
    slotDuration = 1800;
    trainRate = 0.15;
    trainBudget = 300;
    testRate = 0.5;

    def ToString(self):
        return f"(slot={self.slotDuration})(trainR={self.trainRate})(trainB={self.trainBudget}min)(testR={self.testRate})"

class ConfigBurstDataset(ConfigDataset):
    burstTrh = BurstTrh()
    def ToString(self):
        return f"(slot={self.slotDuration})(trainR={self.trainRate})(trainB={self.trainBudget}min)(testR={self.testRate})" + self.burstTrh.ToString();

class ConfigBurstClf:
    def __init__(self, lsenable=True):
        # Initialize with default values based on lsenable
        self.uniPktTolr = 200
        self.pktTolr = 1000
        self.maxUniPkt = 1000  # duplicated with configburstdataset
        self.maxPktIndex = 500
        self.maxTrainItem = 1000
        self.distanceTrh = 0.5
        self.penalty = 0.05
    
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
    baseFolder = "/media/kunling/BigE/IoT/";
    datasetName = DatasetEnum.UNSW201620.name;
    methodName = MethodName.BYTEIOT.name;
    scenario = ExperimentEnum.FEW_SHOTS.name;

    configDataset = ConfigDataset()
    configBurstDataset = ConfigBurstDataset()
    configBurstClf = ConfigBurstClf()
    configShahid = ConfigShahid()
    
    independentArg = IndependentArgEnum.TRAINING_SIZE.name;
    start = 4.0;
    end = 60.0;
    step = 3;

    graphName = "LineChart"
    xLabel = "Slot Duration"
    yLabel = "Accuracy"

    def GetCmWidth(self):
        if(self.datasetName == DatasetEnum.UNSW201620.name):
            return 12;
        elif(self.datasetName == DatasetEnum.BehavIoT2021.name):
            return 23;
        else:
            print("error: datasetName is not UNSW201620 or BehavIoT2021")
            return 0;
        
    def GetCmLen(self):
        if(self.datasetName == DatasetEnum.UNSW201620.name):
            return 8;
        elif(self.datasetName == DatasetEnum.BehavIoT2021.name):
            return 12;
        else:
            print("error: datasetName is not UNSW201620 or BehavIoT2021")
            return 0;

    def GetOutputPath(self):
        return self.baseFolder + "GraphResult/";

    def GetDatasetPath(self):
        return self.baseFolder + self.datasetName + "/";

    def GetCmPath(self):
        self.GetDatasetPath() + "CM/" + self.ToString() + ".pdf"

    def GetFewShotGraphPath(self):
        return self.GetOutputPath() + self.ToString() + ".pdf"

    def GetResultCsvPath(self):
        return self.GetDatasetPath() + self.ToString() + ".csv";




##################### New Naming RUles #####################
    def GetResultFolder(self):
        return self.baseFolder + "Result/" + self.datasetName + "/";

    def GetSummaryMetricCsvPath(self):
        return self.GetResultFolder() + "summaryMetric.csv";

    def GetPredictionCsvPath(self):
        return self.GetResultFolder() + "PredictionCSV/" + self.ToString() + ".csv";


    def GetConfusionMatrixPath(self):
        return self.GetResultFolder() + "ConfusionMatrix/" + self.ToString() + ".pdf";

    def GetComparisonGraphPath(self):
        return self.GetResultFolder() + "Comparison/" + self.datasetName + ".pdf";

    def ToString(self):
        basicString = f"{self.methodName}-{self.scenario}-{self.independentArg}-{self.datasetName}-";
        if self.methodName == MethodName.FSIOT.name:
            basicString += f"{self.configBurstDataset.ToString()}{self.configBurstClf.ToString()}";     
        elif self.methodName == MethodName.BYTEIOT.name:
            basicString += f"{self.configDataset.ToString()}";
        elif self.methodName == MethodName.SHAHID.name:
            basicString += f"{self.configDataset.ToString()}{self.configShahid.ToString()}";     
        elif self.methodName == MethodName.AHMED.name:
            basicString += f"{self.configDataset.ToString()}";
        
        return basicString;


#    ss << methodName << "-" << scenario << "-" ;
#     ss << independentArg << "-" << datasetName << "-";
    # std::stringstream ss;
    # ss << "(slot=" << slotDuration << ")";
    # ss << "(trainR=" << trainRate << ")";
    # ss << "(trainB=" << trainBudget << "min)";
    # ss << "(testR="<< testRate << ")";
    # return ss.str();
# std::string LabSetting::ToString()
# {
#     std::stringstream ss;
#     ss << GetScenarioInfo() << GetTrainTestSplitInfo();
#     switch(methodName)
#     {
#         case MethodEnum::BYTEIOT:
#             break;
#         case MethodEnum::FSIOT:
#             ss << burstTrh.ToString() << clfConfig.ToString();
#             break;
#         case MethodEnum::AHMED:
#             ss << shahidConfig.ToString();
#             break;
#     }
#     return ss.str();
# }

# 6条线的配置
default_series_config = [
    {   # 系列1
        'data': None,
        'label': 'ByteIoT',
        'marker': '*',          # 方形标记
        'color': '#D95319',     # 橙色
        'markerfacecolor': '#D95319',
        'markeredgecolor': '#D95319',
        'linewidth': 3,
        'markersize': 16,   
    },
    {   # 系列2
        'data': None,
        'label': 'BurstIoT',
        'marker': '^',          # 圆形标记
         # red
        'color': '#FFA500',    
        'markeredgecolor': '#FFA500',
        'markerfacecolor': '#FFA500',
        'alpha': 0.7,            # 透明度
        'markersize': 10,   
        'linewidth': 3,
    },
    {   # 系列3
        'data': None,
        'label': 'CloudEdge',
        'marker': 'o',          # 三角形标记
        'color': '#af8215',     # 黄色 EDB120
        'linewidth': 3,
        'markeredgecolor': '#af8215',
        'markerfacecolor': '#af8215',
        'markersize': 10, 
        'alpha': 0.3,            # 透明度

    },
    {   # 系列4
        'data': None,
        'label': 'SmartMesh',
        'marker': 's',          # 星形标记
        'color': '#2C6FB3',     # 蓝色
        'alpha': 0.7,            # 透明度
        'markerfacecolor': '#2C6FB3',
        'markeredgecolor': '#2C6FB3',
        'markersize': 6, 
        'linewidth': 3,
    },
    {   # 系列5
        'data': None,
        'label': 'IoTGuard',
        'marker': 'P',          # 五边形标记
        'color': '#A2142F',     # 深红色
        'markeredgecolor': 'black',
        'markerfacecolor': '#A2142F'
    },
    {   
        'data': None,
        'label': 'ThreatWatch',
        'marker': 'H',          # 六边形标记
        'color': '#17BECF',     # 青色
        'markeredgecolor': 'black',
        'markerfacecolor': '#17BECF'
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
