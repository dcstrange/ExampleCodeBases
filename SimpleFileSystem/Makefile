# Makefile for the filesystem project

CC = gcc
CFLAGS = -Wall -Wextra -std=c11

# Source files
SRCS = constants.h file_types.h utils.c file_ops.c dir_ops.c fs_api.c

# Object files
OBJS = $(SRCS:.c=.o)

# Output binary
TARGET = filesystem

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)