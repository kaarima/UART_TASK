# UART Test Program

This program demonstrates how to initialize, configure, transmit, and receive data over a UART interface on Linux using the `termios` API.

## Features
- Configures UART parameters: baud rate, data bits, parity, stop bits
- Transmits a test message
- Receives data using non-blocking I/O with a timeout
- Prints received data to the console
- Handles errors gracefully

## Build Instructions

1. Open a terminal and navigate to the `uart_project` directory:
   ```sh
   
   ```
2. Build the program using `make`:
   ```sh
   make
   ```

## Run Instructions

Run the program with the UART device path as an argument (e.g., `/dev/ttyS0` or `/dev/ttyUSB0`):

```sh
./uart_test /dev/ttyS0
```

- You may need root permissions or to be in the `dialout` group to access the UART device.
- The program will send a test message and wait up to 5 seconds for incoming data, printing any received data to the console.

## Notes
- Edit the source code if you need to change UART parameters (baud rate, etc.).
- Ensure the device path matches your hardware setup.

## Clean Up
To remove the compiled binary:
```sh
make clean
```
