import matplotlib.pyplot as plt
import numpy as np

# Данные для графика
x = np.linspace(0, 1, 1000)  # 1000 точек от 0 до 1
with open('result.txt', 'r') as file:
    y = [float(line.strip()) for line in file]

# Создание графика
plt.plot(x, y, label="y", color='b', linestyle='-', marker='o')

# Настройка осей
plt.xlabel('X')  # Подпись для оси X
plt.ylabel('Y')  # Подпись для оси Y
plt.title('График решения')  # Заголовок графика

# Добавление легенды
plt.legend()

# Показ графика
plt.grid(True)  # Включаем сетку
plt.show()
