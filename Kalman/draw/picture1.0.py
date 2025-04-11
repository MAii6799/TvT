import matplotlib.pyplot as plt
import numpy as np

# 读取原始数据
original_data = np.loadtxt('homework_data_4.txt')
x_original = original_data[:, 0]
y_original = original_data[:, 1]

# 读取滤波后数据
filtered_data = np.loadtxt('filtered_results_4.txt')
x_filtered = filtered_data[:, 0]
y_filtered = filtered_data[:, 1]

# 创建图形
plt.figure(figsize=(10, 6))  # 设置图形大小

# 绘制原始数据曲线（红色）
plt.plot(x_original, y_original, label='Original data', color='red')

# 绘制滤波后数据曲线（蓝色）
plt.plot(x_filtered, y_filtered, label='Kalman filtered data', color='blue')

# 添加图表元素
plt.xlabel('x')  # x轴标签
plt.ylabel('y')  # y轴标签
plt.title('Comparison of Original and Kalman Filtered Data')  # 图形标题
plt.grid(True)  # 显示网格
plt.legend()  # 显示图例

# 显示图形
plt.show()