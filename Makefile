# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -lm

# Output executable
OUT = sha-256-impl.exe

# Source files
SRC = $(wildcard *.c)  # All C files in the current directory (src)

# Build the executable
all: $(OUT)
	./$(OUT)

$(OUT): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(OUT)

# Clean up generated files
clean:
	rm -f $(OUT)

# Phony targets
.PHONY: all clean

