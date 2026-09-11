CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -D_POSIX_C_SOURCE=200809L

TARGET = shell
SOURCE = shell.c
OBJS = $(SOURCE:.c=.o)

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(SOURCE) -o $(TARGET)
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	 rm -f $(OBJS) $(TARGET) 