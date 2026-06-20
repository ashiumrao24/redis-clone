# Redis Clone

A Redis-inspired in-memory key-value database built in C++17 featuring TTL expiration, LRU cache eviction, persistence, TCP networking, and concurrent client handling.

## Features

### Core Database Operations

* SET key value
* GET key
* DEL key
* EXISTS key
* SIZE

### TTL Expiration

* SET key value EX seconds
* Background cleanup thread
* Lazy expiration during reads

### LRU Cache Eviction

* O(1) cache updates using hash maps and doubly linked lists
* Automatic eviction of least recently used keys when capacity is reached

### Persistence

* Snapshot-based persistence to disk
* Automatic recovery on startup
* Expired keys skipped during recovery

### Networking

* TCP server using POSIX sockets
* Remote client access through netcat/telnet
* Multiple commands per connection

### Concurrency

* Thread-per-client architecture
* Concurrent client handling
* Thread-safe access using mutexes

---

## Architecture

```text
                 TCP Clients
                      |
          -------------------------
          |           |           |
          v           v           v

      Thread 1    Thread 2    Thread 3
           \         |         /
            \        |        /
             \       |       /
                Shared Database
                       |
        --------------------------------
        |              |               |
        v              v               v

      Store          TTL           LRU Cache
                       |
                  Persistence
```

---

## Data Structures

| Component       | Data Structure |
| --------------- | -------------- |
| Key-Value Store | unordered_map  |
| LRU Tracking    | list           |
| Expiry Tracking | unordered_map  |
| Synchronization | mutex          |

---

## Build Instructions

### Compile Server

```bash
g++ server.cpp database.cpp -std=c++17 -pthread -o server
```

### Run Server

```bash
./server
```

Server starts on:

```text
localhost:6379
```

---

## Connect to Server

Using netcat:

```bash
nc localhost 6379
```

---

## Example Usage

```text
SET name Aashi
OK

GET name
Aashi

EXISTS name
1

SIZE
1

DEL name
OK

EXISTS name
0
```

---

## TTL Example

```text
SET temp 123 EX 5
OK

GET temp
123
```

After 5 seconds:

```text
GET temp
NULL
```

---

## Project Structure

```text
redis-clone/
│
├── database.h
├── database.cpp
├── server.cpp
├── README.md
├── .gitignore
└── database.txt
```

---

## Key Learnings

* Hash Tables
* Doubly Linked Lists
* LRU Cache Design
* TCP Networking
* POSIX Sockets
* Multithreading
* Mutex Synchronization
* Persistence and Recovery
* Concurrent Server Design

---

## Future Improvements

* Benchmarking and throughput measurement
* EXPIRE command
* TTL command
* KEYS command
* FLUSHALL command
* Append-Only File (AOF) persistence
* RESP protocol support
* Thread pool architecture

---

## Tech Stack

* C++17
* STL
* POSIX Sockets
* Multithreading
* File I/O
* Linux/macOS Networking APIs

```
```
