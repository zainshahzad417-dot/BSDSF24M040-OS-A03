LDFLAGS = -lreadline
CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
SRC = src/main.c src/shell.c src/execute.c src/history.c src/completion.c src/io.c src/jobs.c src/multitask.c
TARGET = bin/myshell

all: $(TARGET)

$(TARGET): $(SRC) include/shell.h
	mkdir -p bin
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

clean:
	rm -rf bin

.PHONY: all clean
