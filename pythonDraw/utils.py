import matplotlib.pyplot as plt

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


from pathlib import Path

basePath = Path("/media/kunling/BigE/UNSW201620");
nameTemplate = "(slotDur=1800)(trainRate=0.01)(uniTrh=50)(inTrh={2,0}(ouTrh={15,0}byteiot-metrics.txt"

accuracy = ReadAccMetric(basePath.joinpath(nameTemplate))
