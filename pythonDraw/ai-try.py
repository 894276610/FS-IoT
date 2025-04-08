import matplotlib.pyplot as plt
import numpy as np

# 示例数据
x = np.linspace(0, 10, 100)
entropy = np.sin(x) * 2 + 5  # 假设entropy值在3-7范围内
burstNum = np.cos(x) * 1000 + 2000  # 假设burstNum值在1000-3000范围内

# 创建图形和主坐标轴
fig, ax1 = plt.subplots(figsize=(10, 6))

# 绘制第一个数据集（左侧Y轴）
color = 'tab:red'
ax1.set_xlabel('Time (s)')
ax1.set_ylabel('Entropy', color=color)
line1 = ax1.plot(x, entropy, color=color, label='Entropy')
ax1.tick_params(axis='y', labelcolor=color)

# 创建第二个Y轴（右侧）
ax2 = ax1.twinx()
color = 'tab:blue'
ax2.set_ylabel('Burst Number', color=color)
line2 = ax2.plot(x, burstNum, color=color, linestyle='--', label='Burst Number')
ax2.tick_params(axis='y', labelcolor=color)

# 添加图例（合并两个图例）
lines = line1 + line2
labels = [l.get_label() for l in lines]
ax1.legend(lines, labels, loc='upper right')

# 添加标题
plt.title('Entropy and Burst Number over Time')

# 调整布局防止标签重叠
plt.tight_layout()
plt.show()