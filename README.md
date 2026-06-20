# Redis Clone

A Redis-inspired in-memory key-value database built in C++17 featuring TTL expiration, LRU cache eviction, persistence, TCP networking, and concurrent client handling.

## Features

### Core Commands

* SET key value
* GET key
* DEL key
* EXISTS key
* SIZE
* KEYS
* FLUSHALL

### Expiration Commands

* SET key value EX seconds
* EXPIRE key seconds
* TTL key

### LRU Cache Eviction

* O(1) average-time cache operations
* Least Recently Used eviction policy
* Implemented using hash tables and doubly linked lists

### Persistence

* Snapshot-based persistence to disk
* Automatic recovery on startup
* Expired keys skipped during recovery

### Networking

* TCP server using POSIX sockets
* Multiple commands per connection
* Stream-safe command parsing
* Remote client access using netcat

### Concurrency

* Thread-per-client architecture
* Concurrent client handling
* Thread-safe database access using mutexes
* Background expiration cleanup thread

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

### Compile

```bash
g++ server.cpp database.cpp -std=c++17 -pthread -o server
```

### Run

```bash
./server
```

Server starts on:

```text
localhost:6379
```

---

## Connect To Server

```bash
nc localhost 6379
```

---

## Example Session

```text
SET name Aashi
OK

GET name
Aashi

EXISTS name
1

SIZE
1

KEYS
name

EXPIRE name 10
1

TTL name
10
```

After expiration:

```text
GET name
NULL
```

---

## FLUSHALL Example

```text
SET A 1
SET B 2

SIZE
2

FLUSHALL
OK

SIZE
0
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

## Concepts Demonstrated

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

* Performance Benchmarking
* Throughput and Latency Metrics
* Append-Only File (AOF) Persistence
* Thread Pool Architecture
* RESP Protocol Support

---

## Tech Stack

* C++17
* STL
* POSIX Sockets
* Multithreading
* File I/O
* Linux/macOS Networking APIs
