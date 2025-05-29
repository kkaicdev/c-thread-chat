# Compilador
CC = gcc

# Diretórios
SRC_DIR = src
OBJ_DIR = build

# Arquivos fonte
SRCS = $(SRC_DIR)/main.c \
       $(SRC_DIR)/server_net.c \
       $(SRC_DIR)/server_users.c \
       $(SRC_DIR)/server_utils.c

# Arquivos objeto (convertendo src/arquivo.c -> build/arquivo.o)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Executável final
TARGET = chat_server.exe

# Flags de compilação
CFLAGS = -Wall -Wextra -I$(SRC_DIR)
LIBS = -lws2_32

# Regra principal
all: prepare $(TARGET)

# Criação do diretório de objetos, se necessário (compatível com Windows)
prepare:
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)

# Como construir o executável
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LIBS)

# Como compilar cada arquivo .c em .o no build/
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Limpeza
clean:
	@if exist $(OBJ_DIR) del /Q $(OBJ_DIR)\*.o
	@if exist $(TARGET) del /Q $(TARGET)

.PHONY: all clean prepare
