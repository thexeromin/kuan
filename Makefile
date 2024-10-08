CC = gcc
CFLAGS = -Wall -Wextra -std=gnu11
LDFLAGS =

SRCS = main.c socket.c queue.c
OBJS = $(SRCS:.c=.o)
DEPS = socket.h queue.h

TARGET = kuan

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS) 

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

run:
	./$(TARGET)

clean:
	rm -rf $(OBJS) $(TARGET)
