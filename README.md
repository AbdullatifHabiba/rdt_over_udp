# RDT Over UDP Implementation

This project implements a simple client-server application for Reliable Data Transfer (RDT) over UDP (User Datagram Protocol).

## Features

- Reliable data transfer using custom ACK packets.
- Supports Stop-and-Wait and Selective Repeat ARQ protocols.
- Configuration via text files for server and client parameters.
- Basic error handling for socket operations and file I/O.

## Prerequisites

- C Compiler (e.g., GCC)
- POSIX-compatible operating system

## Usage

### Server

1. Compile the server: `make server`
2. Create `server.in` with port, seed, and loss probability.
3. Run: `./server`

### Client

1. Compile the client: `make client`
2. Create `client.in` with server IP, port, and file name.
3. Run: `./client`

## Implementation Details

- Finite State Machine (FSM) approach for efficient state management.
- Modules for client and server FSM logic.
- Packet loss simulation based on probability.

## Limitations

- Assumes a reliable network environment.
- No out-of-order packet handling.
- Lacks encryption and authentication mechanisms.

## References

- UDP Programming Tutorial
- Selective Repeat ARQ
- Stop-and-Wait ARQ

## License

MIT License
