# ft_irc

## Overview

- Functional Internet Relay Chat (IRC) server implemented in C++98.
- Supports basic IRC protocol features including client connections, nicknames, user registration, channels, and channel modes.

## Features

- Written in **C++98**
- Supports standard IRC commands:
  - `/NICK`, `/USER`, `/JOIN`, `/PART`, `/PRIVMSG`, `/PING`, `/PONG`, `/QUIT`
- Channel management:
  - Channel creation and deletion
  - Channel modes: `+i`, `+t`, `+k`, `+o`, `+l`
- Operator commands:
  - `/KICK`, `/MODE`, `/TOPIC`, `/INVITE`
- Client identification via file descriptors
- Proper error handling and numeric replies per IRC protocol

## Getting Started

### Prerequisites

- A POSIX-compliant system (Linux, macOS)
- A C++98-compatible compiler (e.g., `g++`)

### Compilation

Run the makefile. This will build an executable named ircserv.

### Running the Server
```bash
./ircserv <port> <password>
