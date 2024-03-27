# PokemonGame
 

## Overview

Learning all about Client and Server and real time communication using sockets in C++.

This pokemon networking card game system facilitates the interaction between clients and a server to manage card transactions, user sessions, and real-time communication over a network. Designed with a focus on scalability and robustness, the system supports multiple clients, allowing users to buy, sell, deposit, and list cards in a simulated card trading environment.

## Features

- **User Authentication**: Secure login and session management for users.
- **Real-time Communication**: Sockets enable real-time data exchange between clients and the server.
- **Database Integration**: SQLite database for persistent storage of user and card data.
- **Command Handling**: Extensible command pattern for handling various user requests.

## Components

- **Server**: Manages client connections, sessions, and executes client commands.
- **Client**: Allows users to connect to the server, send commands, and receive responses.
- **Database**: Stores user information, card inventory, and transaction records.
- **Commands**: A set of commands (e.g., Login, Buy, Sell, Deposit, Balance, List, Lookup) that clients can execute.

## Prerequisites

To compile and run this project, you need the following:

- C++ Compiler (Supporting C++11 or higher)
- Windows Socket API (WinSock2 for Windows)
- SQLite3

## Compilation

### Server

```bash
g++ -o Server.exe Server.cpp NetworkHandler.cpp db.cpp UserClass.cpp ... main.cpp  -lws2_32 -lsqlite3
```

### Client

```bash
g++ -o Client.exe clientCode.cpp -lws2_32
```

## Running the Application

### Start the Server

```bash
./Server.exe
```

### Run a client

```bash
./Client.exe
```

## Usage

- After starting the server, run the client application. You'll be prompted to log in. After logging in, you can execute commands as outlined in the commands section.

## Commands

- LOGIN <username> <password>: Log in to the system.
- BUY <card_name> <type> <rarity> <price> <count> <ownerID>: Purchase a card.
- SELL <card_name> <quantity> <price> <ownerID>: Sell a card.
- DEPOSIT <amount>: Deposit funds into your account.
- BALANCE: Check your current balance.
- LIST: List all available cards.
- LOOKUP <query>: Look up cards based on a query.
- LOGOUT: Log out of the system.

## Architecture

- This system uses a client-server model where the server handles multiple client connections, processing commands in real-time. Data persistence is managed through an SQLite database, ensuring data integrity and consistency.