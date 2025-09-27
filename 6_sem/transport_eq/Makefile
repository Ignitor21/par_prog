# Компилятор
CC = g++
# Флаги компиляции
CFLAGS = -O2
# Имя исполняемого файла
TARGET = usual

# Все исходные файлы .c
SRCS = usual.c utils.c
# Все объектные файлы .o (заменяем .c на .o)
OBJS = $(SRCS:.c=.o)

# Сборка по умолчанию
all: $(TARGET)

# Линковка объектных файлов в исполняемый
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Компиляция каждого .c файла в .o
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Очистка
clean:
	rm -f $(OBJS) $(TARGET)

# Указываем, что цель 'clean' не создает файл
.PHONY: clean all
