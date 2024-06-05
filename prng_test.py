import numpy as np
import matplotlib.pyplot as plt
from scipy.stats import chisquare

# Чтение сгенерированных чисел из файла
numbers = np.loadtxt('random_numbers.txt', dtype=np.uint32)

# Гистограмма
plt.hist(numbers, bins=50, density=True)
plt.title("Гистограмма сгенерированных чисел")
plt.show()

# 2D точечная диаграмма
plt.scatter(numbers[:-1], numbers[1:], s=1)
plt.title("2D точечная диаграмма последовательных чисел")
plt.show()

# Chi-square тест
observed_freq, _ = np.histogram(numbers, bins=50)
expected_freq = np.ones_like(observed_freq) * len(numbers) / 50
chi2_stat, p_value = chisquare(observed_freq, expected_freq)
print(f"Chi-square stat: {chi2_stat}, p-value: {p_value}")

# Автокорреляция
autocorr = np.correlate(numbers, numbers, mode='full')
autocorr = autocorr[autocorr.size // 2:]
plt.plot(autocorr[:100])
plt.title("Автокорреляция последовательности")
plt.show()
