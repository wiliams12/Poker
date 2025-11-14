# Compiler
CC = gcc

# Compiler flags (add -Wall for warnings)
CFLAGS = -Wall

# Final program name
TARGET = main

# Build the program from main.c and lib.c
$(TARGET): main.o lib.o
	$(CC) $(CFLAGS) -o $(TARGET) main.o lib.o

# Compile main.c into main.o
main.o: main.c lib.h
	$(CC) $(CFLAGS) -c main.c

# Compile lib.c into lib.o
lib.o: lib.c lib.h
	$(CC) $(CFLAGS) -c lib.c

# Run the program
run: $(TARGET)
	./$(TARGET)

# Clean compiled files
clean:
	rm -f *.o $(TARGET)
