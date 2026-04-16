/*
 * uart_test.c
 *
 * Demonstrates UART initialization, configuration, transmission, and reception on Linux using termios.
 *
 * Usage: ./uart_test /dev/ttyS0
 *
 * Author: [Your Name]
 * Date: 2026-04-16
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <sys/select.h>

#define TEST_MESSAGE "Hello from UART!\n"
#define READ_TIMEOUT_SEC 5
#define READ_BUF_SIZE 256

// Configure UART parameters: baud rate, data bits, parity, stop bits
int configure_uart(int fd, speed_t baudrate, int databits, int parity, int stopbits) {
    struct termios tty;
    if (tcgetattr(fd, &tty) != 0) {
        perror("tcgetattr");
        return -1;
    }

    // Set baud rate
    cfsetospeed(&tty, baudrate);
    cfsetispeed(&tty, baudrate);

    // Set data bits
    tty.c_cflag &= ~CSIZE;
    switch (databits) {
        case 5: tty.c_cflag |= CS5; break;
        case 6: tty.c_cflag |= CS6; break;
        case 7: tty.c_cflag |= CS7; break;
        case 8: tty.c_cflag |= CS8; break;
        default:
            fprintf(stderr, "Unsupported data bits: %d\n", databits);
            return -1;
    }

    // Set parity
    if (parity == 0) {
        tty.c_cflag &= ~PARENB;
    } else if (parity == 1) {
        tty.c_cflag |= PARENB;
        tty.c_cflag |= PARODD;
    } else if (parity == 2) {
        tty.c_cflag |= PARENB;
        tty.c_cflag &= ~PARODD;
    } else {
        fprintf(stderr, "Unsupported parity: %d\n", parity);
        return -1;
    }

    // Set stop bits
    if (stopbits == 1) {
        tty.c_cflag &= ~CSTOPB;
    } else if (stopbits == 2) {
        tty.c_cflag |= CSTOPB;
    } else {
        fprintf(stderr, "Unsupported stop bits: %d\n", stopbits);
        return -1;
    }

    tty.c_cflag |= CLOCAL | CREAD; // Enable receiver, ignore modem control lines
    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // Raw input
    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // No software flow control
    tty.c_oflag &= ~OPOST; // Raw output

    // Set read timeout (non-blocking)
    tty.c_cc[VMIN] = 0;
    tty.c_cc[VTIME] = 0;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        perror("tcsetattr");
        return -1;
    }
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <uart_device>\n", argv[0]);
        return 1;
    }
    const char *uart_dev = argv[1];
    int fd = open(uart_dev, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd < 0) {
        perror("open");
        fprintf(stderr, "Failed to open UART device: %s\n", uart_dev);
        return 1;
    }

    // Configure UART: 115200 baud, 8 data bits, no parity, 1 stop bit
    if (configure_uart(fd, B115200, 8, 0, 1) != 0) {
        close(fd);
        return 1;
    }

    // Transmit test message
    ssize_t written = write(fd, TEST_MESSAGE, strlen(TEST_MESSAGE));
    if (written < 0) {
        perror("write");
        close(fd);
        return 1;
    } else {
        printf("Sent: %s", TEST_MESSAGE);
    }

    // Prepare to receive data with timeout
    fd_set readfds;
    struct timeval timeout;
    char buf[READ_BUF_SIZE];
    printf("Waiting to receive data (timeout: %d seconds)...\n", READ_TIMEOUT_SEC);
    FD_ZERO(&readfds);
    FD_SET(fd, &readfds);
    timeout.tv_sec = READ_TIMEOUT_SEC;
    timeout.tv_usec = 0;
    int rv = select(fd + 1, &readfds, NULL, NULL, &timeout);
    if (rv < 0) {
        perror("select");
        close(fd);
        return 1;
    } else if (rv == 0) {
        printf("No data received within timeout.\n");
    } else {
        ssize_t n = read(fd, buf, sizeof(buf) - 1);
        if (n < 0) {
            perror("read");
        } else if (n == 0) {
            printf("No data available.\n");
        } else {
            buf[n] = '\0';
            printf("Received (%zd bytes): %s\n", n, buf);
        }
    }

    close(fd);
    return 0;
}
