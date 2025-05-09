import matplotlib.pyplot as plt
import numpy as np

plt.rcParams['pdf.fonttype'] = 42
plt.rcParams['ps.fonttype'] = 42

# 数据配置
models = [
    "Ahmed", "ByteIoT", "AutoIoT", "HSGAN-IoT", "GraphIoT", "Shigeng",
    "ENSIOT", "IoTProfile", "Yuming", "FS-IoT",
    "Sivathan-17", "Sivathan-19", "Sun", "ETEI"
]
train_sizes = [276,410,672, 76.8, 864, 240, 480, 480, 480, 1, 864, 48, 864, 768]  # 训练数据量
accuracies = [ 99,99,
    99.68, 98.63, 98.84, 97.65,
    98.80, 99.90, 97.00, 99.94,
    97, 99.28, 99.56, 99.91
]

# 可视化设置
colors = ['#1f77b4' if model != "FS-IoT" else '#ff0000' for model in models]
sizes = [120 if model != "FS-IoT" else 200 for model in models]  # 突出显示FS-IoT

# 创建画布
plt.figure(figsize=(10, 6))
scatter = plt.scatter(
    train_sizes, 
    accuracies,
    c=colors,
    s=sizes,
    alpha=0.8,
    edgecolors='w',
    linewidths=1
)

# 添加数据标签
for i, (x, y, model) in enumerate(zip(train_sizes, accuracies, models)):
    
    offsetX = (-19 if model == "Ahmed" else 0)
    offsetX = (4 if model == "ByteIoT" else offsetX)
    offsetX = (-40 if model == "IoTProfile"  else offsetX)
    offsetX = (19 if model == "ETEI"  else offsetX)


    offsetX = (-33 if model == "AutoIoT"  else offsetX)
    offsetX = (-17 if model == "Sun"  else offsetX)

    offsetX = (-33 if model == "ENSIOT"  else offsetX)

    offsetX = (-10 if model == "GraphIoT"  else offsetX)
    offsetX = (-18 if model == "Sivathan-17"  else offsetX)

    offset = (-20 if model == "FS-IoT"  else 10)  # 差异化标签偏移
    offset = (-8 if model == "AutoIoT" else offset)
    offset = (-8 if model == "Sun" else offset)

    offset = (-20 if model == "Sivathan-17" else offset)

  

    offset = (-8 if model == "ENSIOT" else offset)
    offset = (-8 if model == "IoTProfile" else offset)
    offset = (-8 if model == "ETEI" else offset)



    plt.annotate(
        model,
        (x, y),
        textcoords="offset points",
        xytext=(offsetX, offset),
        ha='center',
        fontsize=15
    )

# 坐标轴设置
plt.xscale('log')  # 对数坐标处理量级差异
plt.xticks(fontsize=18)
plt.yticks(fontsize=18)
# plt.xticks([1, 2,3,100, 300, 500, 700,900], ['1', '2', '3', '100', '300', '500', '700', '900'])
plt.xlabel("Training Samples Per Class", fontsize=20)
plt.ylabel("Accuracy (%)", fontsize=20)
plt.ylim(96, 100)

# 参考线辅助阅读
plt.axhline(y=99.91, color='gray', linestyle='--', lw=0.8, alpha=0.7)
plt.axvline(x=100, color='gray', linestyle='--', lw=0.8, alpha=0.7)

# 图例和标题
# plt.title("Accuracy vs Training Data Size on UNSW Dataset", pad=15)
plt.grid(True, alpha=0.3, which='both')

plt.tight_layout()
plt.savefig('scatter_plot.pdf')