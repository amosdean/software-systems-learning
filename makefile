CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -D_POSIX_C_SOURCE=200809L -g

ifeq ($(OS),Windows_NT)
	RM = del /Q
	TARGET_SUFFIX = .exe
	CLEAN_FILES = $(subst /,\,$(OBJS))
else
	RM = rm -f
	TARGET_SUFFIX =
	CLEAN_FILES = $(OBJS)
endif

TARGET = main
SOURCE = ./SQL_Engine/main.c ./SQL_Engine/lexer.c
OBJS = $(SOURCE:.c=.o)

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET) ./SQL_Engine/example.sql

clean:
	$(RM) $(CLEAN_FILES) $(TARGET)$(TARGET_SUFFIX)