# Corrected Makefile for the filesystem project

CC = gcc
CFLAGS = -Wall -Wextra -std=c11

# Source files
SRCS = utils.c file_ops.c dir_ops.c fs_api.c main.c

OBJS = $(SRCS:.c=.o)

# Header files
HEADERS = constants.h file_types.h

# Output binary
TARGET = filesystem

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

%.o: %.c $(HEADERS)

	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)