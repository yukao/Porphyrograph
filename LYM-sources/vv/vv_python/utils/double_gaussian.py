# Imports
import numpy as np
import matplotlib.pyplot as plt
from scipy.stats import norm

# Data
mean = 0.5

standard_deviation = 0.15


x_values = np.arange(0, 1, 0.01)

y_values = norm(mean, standard_deviation)
# print(y_values)

print(y_values.pdf(x_values))
print(y_values.pdf(x_values)[50])

plt.plot(x_values, y_values.pdf(x_values) / 2.65)
plt.show()

