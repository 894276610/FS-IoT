import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

plt.rcParams['pdf.fonttype'] = 42
plt.rcParams['ps.fonttype'] = 42


def plotBurstWrapper(csvFile, saveFile, Width, Len):
    df = pd.read_csv(csvFile)

    # 按size排序
    df_sorted = df.sort_values(by='uniBurstNum', ascending=False)
    df_sorted.reset_index(drop=True, inplace=True)
    # 删除 burstNum 最大的
    df_sorted = df_sorted[df_sorted['burstNum'] != df_sorted['burstNum'].max()]

    nameList = df_sorted['name'].tolist()
    # burstNumList = df_sorted['burstNum'].tolist()
    uniBurstNumList = df_sorted['uniBurstNum'].tolist()
     # 绘图设置
    plt.figure(figsize=(Width, Len))

    # 修改点：使用单一蓝色系的渐变（从浅蓝到深蓝）
    colors = plt.cm.Blues(np.linspace(0.65, 0.4, len(nameList)))  # 缩小颜色范围

    # 绘制水平柱状图（避免长文本重叠）
    plt.barh(nameList, uniBurstNumList, color=colors, edgecolor='black', linewidth=0.5)
    # 数值大的在上方
    plt.gca().invert_yaxis();
    plt.title("", fontsize=14, pad=20)
    plt.xlabel("Packet Burst Fingerprint Total", fontsize=20)
    # plt.ylabel("Device Name", fontsize=20)
    plt.xticks(fontsize=15)
    plt.yticks(fontsize=15)

    plt.grid(axis='x', linestyle='--', alpha=0.6)

    # 添加数据标签
    for index, value in enumerate(uniBurstNumList):
        plt.text(value + 0.5, index, str(value), va='center', fontsize=14)

    plt.tight_layout()  # 自动调整布局
    plt.savefig(saveFile)

if __name__ == '__main__':
    baseFolder = "/media/kunling/dpan/IoT/Result/UNSW201620/Division/"
    fileName = "division-FSIOT-DIVISION-TRAINING_SIZE-UNSW201620-(slot=1800)(trainR=0.15)(trainB=30min)(testR=0.5)(uniTrh=1000)(inTrh=2s)(ouTrh=15s)(lsenable=1)(Tuni=200)(Tpkt=1000)(mUniPkt=1000)(mPktIdx=500)(dTrh=0.45)(maxTrainIt=1000)(pen=0.05).csv";
    inputPath = baseFolder + fileName;
    outPath = baseFolder + fileName + ".pdf";
    plotBurstWrapper(inputPath, outPath, 10, 8) 
    exit(0);

    # 数据预处理
    data = [
        ("HPPrinter",15), ("TPCloudCam",11), ("BloodPressureMeter",1),
        ("PhotoFrame",12), ("SmartScale",11), ("SleepSensor",7),
        ("TPSmartPlug",5), ("BabyMonitor",10), ("TribySpeaker",8),
        ("SmokeAlarm",12), ("Welcome",21), ("WeatherStat",7),
        ("BelkinSwitch",14), ("SmartThings",13), ("SmartBulb",26),
        ("iHome",24), ("BelkinMotion",18), ("Dropcam",11),
        ("InsteonCam",26), ("AmazonEcho",35), ("SamsungCam",32)
    ]

    # 按size排序
    data_sorted = sorted(data, key=lambda x: x[1], reverse=True)
    devices = [item[0] for item in data_sorted]
    sizes = [item[1] for item in data_sorted]

    # 绘图设置
    plt.figure(figsize=(10, 8))

    # 修改点：使用单一蓝色系的渐变（从浅蓝到深蓝）
    colors = plt.cm.Blues(np.linspace(0.65, 0.4, len(devices)))  # 缩小颜色范围

    # 绘制水平柱状图（避免长文本重叠）
    plt.barh(devices, sizes, color=colors, edgecolor='black', linewidth=0.5)
    plt.gca().invert_yaxis()  # 数值大的在上方
    plt.title("", fontsize=14, pad=20)
    plt.xlabel("Packet Burst Fingerprint Total", fontsize=20)
    # plt.ylabel("Device Name", fontsize=20)
    plt.xticks(fontsize=15)
    plt.yticks(fontsize=15)

    plt.grid(axis='x', linestyle='--', alpha=0.6)

    # 添加数据标签
    for index, value in enumerate(sizes):
        plt.text(value + 0.5, index, str(value), va='center', fontsize=14)

    plt.tight_layout()  # 自动调整布局
    plt.savefig('unsw-fingerprints.pdf')

