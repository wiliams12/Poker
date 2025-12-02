# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -g

# Final program name
TARGET = main

# Object files
OBJS = main.o lib.o helpers.o bit.o

# Build final program
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Compile main.c
main.o: main.c lib.h helpers.h bit.h
	$(CC) $(CFLAGS) -c main.c

# Compile lib.c
lib.o: lib.c lib.h helpers.h bit.h
	$(CC) $(CFLAGS) -c lib.c

# Compile helpers.c
helpers.o: helpers.c helpers.h lib.h bit.h
	$(CC) $(CFLAGS) -c helpers.c

bit.o: bit.c bit.h
	$(CC) $(CFLAGS) -c bit.c

# Run
run: $(TARGET)
	./$(TARGET)

# Clean
clean:
	rm -f *.o $(TARGET)
