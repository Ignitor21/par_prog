#!/usr/bin/python3

import numpy as np
import matplotlib.pyplot as plt

def parse_file(filename):
    with open(filename, 'r') as file:
        data = file.read().split()
    return np.array(data, dtype=float)

u = parse_file('u.txt')
x = parse_file('x.txt')

if len(u) != len(x):
    raise ValueError("Файлы имеют разное количество чисел!")

plt.figure(figsize=(10, 6)) 
plt.plot(x, u, 'b-', label='u(x)')  
plt.scatter(x, u, color='red', s=20, label='Точки')
plt.xlabel('x')
plt.ylabel('u')
plt.title('График зависимости u(x)')
plt.grid(True)
plt.legend()
plt.show()
