CC = gcc
FLAGS = -Wall -Wextra -pedantic -ggdb
TARGET = main
TARGET_DIR = ./build

SRCS = main.c serial_file.c seq_file.c
OBJS = $(SRCS:%.c=$(TARGET_DIR)/%.o)

all: $(TARGET_DIR) $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(FLAGS) $^ -o $@

$(TARGET_DIR)/%.o: %.c
	$(CC) $(FLAGS) -c $< -o $@

$(TARGET_DIR):
ifeq ($(wildcard $(TARGET_DIR)),)
	mkdir -p $(TARGET_DIR)
endif

clear:
	rm -rf $(TARGET_DIR) $(TARGET)
