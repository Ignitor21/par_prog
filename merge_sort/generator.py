import sys
import random

def generate_random_numbers(n, filename="input.txt"):
    with open(filename, 'w') as f:
        for _ in range(n):
            f.write(f"{random.randint(0, 1000)}\n")  # Числа от 0 до 1000

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Использование: python generate_numbers.py <n>")
        sys.exit(1)
    
    try:
        n = int(sys.argv[1])
        if n <= 0:
            raise ValueError
    except ValueError:
        print("Ошибка: n должно быть положительным целым числом")
        sys.exit(1)
    
    generate_random_numbers(n)
    print(f"Файл 'input.txt' с {n} случайными числами успешно создан!")
