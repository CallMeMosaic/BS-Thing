# Compiler
CC = gcc

# Flags
CFLAGS = -Wall -Wextra -g
LDFLAGS = -lcurl  # Link libcurl for HTTP requests

# Directories
SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build

# Target executable
TARGET = server

# Source files
SRCS = $(wildcard $(SRC_DIR)/*.c)

# Object files
OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

# Default target
all: $(TARGET)

# Create build directory
$(shell mkdir -p $(BUILD_DIR))

# Compile object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Link object files into executable
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

# Clean build files
clean:
	rm -rf $(BUILD_DIR) $(TARGET)

.PHONY: all clean