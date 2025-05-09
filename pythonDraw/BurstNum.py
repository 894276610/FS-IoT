import matplotlib.pyplot as plt
import numpy as np

plt.rcParams['pdf.fonttype'] = 42
plt.rcParams['ps.fonttype'] = 42
# 数据预处理
data = [
    ("microwave", 4), ("t-wemo-plug", 10), ("wansview-cam-wired", 12),
    ("philips-bulb", 6), ("microseven-camera", 3), ("gosund-bulb1", 6),
    ("aqara-hub", 11), ("bulb1", 2), ("wink-hub2", 8), ("switchbot-hub", 3),
    ("govee-led1", 6), ("ring-chime1", 8), ("sousvide", 4), ("dlink-camera", 10),
    ("ring-doorbell", 9), ("keyco-air", 4), ("thermopro-sensor", 7), 
    ("tplink-bulb", 4), ("homepod", 19), ("dlink-mov", 5), 
    ("smartlife-bulb", 3), ("meross-dooropener", 8), ("tplink-plug", 4),
    ("nest-tstat", 5), ("ubell-doorbell", 11), ("tuya-camera", 8),
    ("echodot", 23), ("t-philips-hub", 49), ("lefun-cam-wired", 39),
    ("icsee-doorbell", 9), ("homepod-mini1", 15), ("amazon-plug", 6),
    ("ikettle", 11), ("ring-camera", 5), ("brewer", 11), ("yi-camera", 4),
    ("ikea-hub", 4), ("smartthings-hub", 31), ("magichome-strip", 3),
    ("google-home-mini", 28), ("echodot3a", 25), ("fridge", 31),
    ("wyze-cam", 29), ("echoflex1", 22), ("echodot4b", 29),
    ("google-nest-mini1", 39), ("echoplus", 52), ("echospot", 57),
    ("echoshow5", 57)
]

# 按size排序
data_sorted = sorted(data, key=lambda x: x[1], reverse=True)
devices = [item[0] for item in data_sorted]
sizes = [item[1] for item in data_sorted]

# 绘图设置
plt.figure(figsize=(10, 12))

# 修改点：使用单一蓝色系的渐变（从浅蓝到深蓝）
colors = plt.cm.Blues(np.linspace(0.65, 0.4, len(devices)))  # 缩小颜色范围

# 绘制水平柱状图（避免长文本重叠）
plt.barh(devices, sizes, color=colors, edgecolor='black', linewidth=0.5)
plt.gca().invert_yaxis()  # 数值大的在上方

ax = plt.gca()
ax.set_ylim(len(devices) - 0.5 , -0.5) 

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
plt.savefig('iotbehav-fingerprints.pdf',bbox_inches='tight')

# import matplotlib.pyplot as plt
# import numpy as np

# # 数据预处理
# data = [
#     ("microwave", 4), ("t-wemo-plug", 10), ("wansview-cam-wired", 12),
#     ("philips-bulb", 6), ("microseven-camera", 3), ("gosund-bulb1", 6),
#     ("aqara-hub", 11), ("bulb1", 2), ("wink-hub2", 8), ("switchbot-hub", 3),
#     ("govee-led1", 6), ("ring-chime1", 8), ("sousvide", 4), ("dlink-camera", 10),
#     ("ring-doorbell", 9), ("keyco-air", 4), ("thermopro-sensor", 7), 
#     ("tplink-bulb", 4), ("homepod", 19), ("dlink-mov", 5), 
#     ("smartlife-bulb", 3), ("meross-dooropener", 8), ("tplink-plug", 4),
#     ("nest-tstat", 5), ("ubell-doorbell", 11), ("tuya-camera", 8),
#     ("echodot", 23), ("t-philips-hub", 49), ("lefun-cam-wired", 39),
#     ("icsee-doorbell", 9), ("homepod-mini1", 15), ("amazon-plug", 6),
#     ("ikettle", 11), ("ring-camera", 5), ("brewer", 11), ("yi-camera", 4),
#     ("ikea-hub", 4), ("smartthings-hub", 31), ("magichome-strip", 3),
#     ("google-home-mini", 28), ("echodot3a", 25), ("fridge", 31),
#     ("wyze-cam", 29), ("echoflex1", 22), ("echodot4b", 29),
#     ("google-nest-mini1", 39), ("echoplus", 52), ("echospot", 57),
#     ("echoshow5", 57)
# ]

# # 按size排序
# data_sorted = sorted(data, key=lambda x: x[1], reverse=True)
# devices = [item[0] for item in data_sorted]
# sizes = [item[1] for item in data_sorted]

# # 绘图设置
# plt.figure(figsize=(20, 40))
# colors = plt.cm.viridis(np.linspace(0.2, 0.8, len(devices)))

# # 绘制水平柱状图（避免长文本重叠）
# plt.barh(devices, sizes, color=colors)
# plt.gca().invert_yaxis()  # 数值大的在上方
# plt.title("", fontsize=14, pad=20)
# plt.xlabel("Packet Burst Fingerprint Total", fontsize=20)
# plt.ylabel("Device Name", fontsize=20)
# plt.xticks(fontsize=13)
# plt.yticks(fontsize=13)

# plt.grid(axis='x', linestyle='--')

# # 添加数据标签
# for index, value in enumerate(sizes):
#     plt.text(value + 0.5, index, str(value), va='center')

# plt.show()