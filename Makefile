CC = gcc
CFLAGS = -Wall -O2
TARGET = uart_test
SRC = uart_test.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)
