CC = gcc
CFLAGS = -Wall -Wextra -O2 -I./lib

SRC = main.c lib/rudy_threadpool.c
OBJ = $(SRC:.c=.o)
TARGET = main

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
