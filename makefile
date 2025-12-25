# Compiler and Flags
CC = gcc
CFLAGS = -Wall -g
SRC_DIR = c-src
TARGET = main

# Object files
OBJS = main.o lib.o helpers.o bit.o

# 1. Link the final executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# 2. Compile objects from the c-src directory
# The syntax '%.o: $(SRC_DIR)/%.c' handles all files automatically
%.o: $(SRC_DIR)/%.c $(SRC_DIR)/lib.h $(SRC_DIR)/helpers.h $(SRC_DIR)/bit.h
	$(CC) $(CFLAGS) -c $< -o $@

# Special case for bit.o if it has fewer dependencies
bit.o: $(SRC_DIR)/bit.c $(SRC_DIR)/bit.h
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: run clean

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f *.o $(TARGET)