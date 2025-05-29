CC = gcc

SRC_DIR = src
OBJ_DIR = build
INC_DIR = include

SRCS = $(SRC_DIR)/main.c \
       $(SRC_DIR)/client_net.c \
       $(SRC_DIR)/client_utils.c

OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

TARGET = chat_client.exe

CFLAGS = -Wall -Wextra -I$(INC_DIR)
LIBS = -lws2_32

all: prepare $(TARGET)

prepare:
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@if exist $(OBJ_DIR) del /Q $(OBJ_DIR)\*.o
	@if exist $(TARGET) del /Q $(TARGET)

.PHONY: all clean prepare
