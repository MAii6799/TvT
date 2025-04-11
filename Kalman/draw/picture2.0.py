import matplotlib.pyplot as plt
import numpy as np

# 读取CSV数据文件（假设格式：第一列时间/日期，第二列价格）
try:
    data = np.loadtxt('stock_prices.csv', delimiter=',', skiprows=1)  # 跳过标题行
except:
    data = np.loadtxt('stock_prices.csv', delimiter=',')  # 若无标题行则直接读取

# 提取数据列
x = data[:, 0]  # 第一列作为x轴（时间/交易日序号）
y = data[:, 1]  # 第二列作为y轴（股票价格）

# 创建专业级股票图表
plt.figure(figsize=(12, 6), dpi=100)  # 设置画布尺寸和分辨率

# 绘制股价线图（蓝色实线，带圆点标记）
plt.plot(x, y, 'b-o', linewidth=1.5, markersize=4, 
         label='股价走势', alpha=0.8)

# 图表装饰元素
plt.title('Stock Price', fontsize=15, pad=20)  # 标题及间距
plt.xlabel('Day', fontsize=12)  # x轴标签
plt.ylabel('Price', fontsize=12)   # y轴标签

# 添加网格线（浅灰色虚线）
plt.grid(True, linestyle='--', linewidth=0.5, color='gray', alpha=0.4)

# 自动旋转x轴标签（防止日期重叠）
plt.xticks(rotation=45)

# 紧凑布局（防止标签被裁剪）
plt.tight_layout()

# 显示图例（右上角位置）
plt.legend(loc='upper right', framealpha=0.5)

# 显示图表
plt.show()