CC = gcc
CFLAGS = -Wall -O2 -Werror=implicit-function-declaration -std=c99
OBJ = gb_emu.x
OUT_DIR = build
SRC_DIR = src
O_FILES = $(OUT_DIR)/main.o $(OUT_DIR)/test.o $(OUT_DIR)/registers.o $(OUT_DIR)/cpu.o

all: $(O_FILES)
	$(CC) $(CFLAGS) $(O_FILES) -o $(OBJ) -lm

$(OUT_DIR)/main.o: $(SRC_DIR)/main.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/main.c -o $(OUT_DIR)/main.o

$(OUT_DIR)/registers.o: $(SRC_DIR)/registers.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/registers.c -o $(OUT_DIR)/registers.o

$(OUT_DIR)/cpu.o: $(SRC_DIR)/cpu.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/cpu.c -o $(OUT_DIR)/cpu.o

$(OUT_DIR)/test.o: $(SRC_DIR)/test.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/test.c -o $(OUT_DIR)/test.o

clean:
	rm -f $(OUT_DIR)/*.o

test:
	make
	make clean
	./$(OBJ)

exec:
	make
	./$(OBJ)
