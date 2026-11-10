CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
SRC = src/main.c src/shell.c src/execute.c src/history.c
TARGET = bin/myshell

all: $(TARGET)

$(TARGET): $(SRC) include/shell.h
	mkdir -p bin
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -rf bin

.PHONY: all clean
