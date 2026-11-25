CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2 -Iinclude
TARGET = electrodb

SRCS = src/main.c src/storage.c src/database.c src/calculator.c src/knowledge.c src/cli.c src/server.c
OBJS = $(SRCS:.c=.o)

ifeq ($(OS),Windows_NT)
	LDFLAGS = -lws2_32
	TARGET_BIN = $(TARGET).exe
else
	LDFLAGS = -lm
	TARGET_BIN = $(TARGET)
endif

all: $(TARGET_BIN)

$(TARGET_BIN): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET_BIN) $(OBJS) $(LDFLAGS)

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o $(TARGET) $(TARGET).exe

run: $(TARGET_BIN)
	./$(TARGET_BIN)

cli: $(TARGET_BIN)
	./$(TARGET_BIN) --cli

.PHONY: all clean run cli
