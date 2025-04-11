import matplotlib.pyplot as plt
import numpy as np

# 设置中文显示
plt.rcParams['font.sans-serif'] = ['SimHei']
plt.rcParams['axes.unicode_minus'] = False

# 读取含标题行的CSV文件（跳过第1行）
raw_data = np.loadtxt('stock_prices.csv', delimiter=',', skiprows=1)
filtered_data = np.loadtxt('predicted_prices.csv', delimiter=',', skiprows=1)

# 提取数据列（假设格式：x值,原始值,滤波值）
x = raw_data[:, 0]       # 第1列作为x轴
y_raw = raw_data[:, 1]    # 第2列原始数据
y_filtered = filtered_data[:, 1]  # 第2列滤波数据

# 创建对比图表
plt.figure(figsize=(12, 6))

# 绘制原始信号（红色半透明）
plt.plot(x, y_raw, 'r-', alpha=0.6, linewidth=2, label='original')

# 绘制滤波信号（蓝色实线）
plt.plot(x, y_filtered, 'b-', linewidth=1.5, label='predicted')

# 填充差异区域
plt.fill_between(x, y_raw, y_filtered, 
                where=(y_raw > y_filtered),
                color='red', alpha=0.1)
plt.fill_between(x, y_raw, y_filtered,
                where=(y_raw <= y_filtered),
                color='blue', alpha=0.1)

# 图表装饰
plt.title('predicted', fontsize=16, pad=20)
plt.legend(loc='best')

plt.tight_layout()
plt.show()