# Wall is used to show all warnings
# Wextra is used to enable extra warnings
# Werror is used to treat warnings as errors
# std=c11 is used to specify the C11 standard - the latest version of the C language
# CFLAGS is used to store the flags

CC = clang

CFLAGS = -Wall -Wextra -Werror -I/usr/include/SDL2

LDFLAGS = -lSDL2 -lm
DEBUG_FLAGS = -g

#  TARGET is used to store the name of the executable
TARGET = renderer

# source directries
SRC_DIR = ./src
ERRORS_DIR = ./src/errors
DISPLAY_DIR = ./src/display
DRAW_DIR = ./src/draw
GEOMETRY_DIR = ./src/geometry
UTILS_DIR = ./src/utils

# Source files are stored in the SRC variable 
# The main.c file is stored in the src directory
SRCS = $(wildcard $(SRC_DIR)/*.c) $(ERRORS_DIR)/errors.c $(wildcard $(DISPLAY_DIR)/*.c) $(wildcard $(DRAW_DIR)/*.c) $(wildcard $(GEOMETRY_DIR)/*.c) $(wildcard $(UTILS_DIR)/*.c)

# Object files
OBJS = $(SRCS:.c=.o)

# Default rule
all: $(TARGET)

# Rule to build the target executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Rule to build object files
$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(ERRORS_DIR)/%.o: $(ERRORS_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Debug target
debug: CFLAGS += $(DEBUG_FLAGS)
debug: clean $(TARGET)

run: $(TARGET)
	./$(TARGET)

# Clean rule
clean:
	rm -f $(OBJS) $(TARGET)

# Phony targets
.PHONY: all clean