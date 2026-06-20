# Redis Clone

A Redis-inspired in-memory key-value database built in **C++17** featuring **TCP networking**, **multithreading**, **TTL expiration**, **LRU cache eviction**, **persistence**, and **performance benchmarking**.

---

## Features

### Core Commands

| Command       | Description              |
| ------------- | ------------------------ |
| SET key value | Store a key-value pair   |
| GET key       | Retrieve a value         |
| DEL key       | Delete a key             |
| EXISTS key    | Check if a key exists    |
| SIZE          | Get total number of keys |
| KEYS          | List all keys            |
| FLUSHALL      | Remove all keys          |

### Expiration Commands

| Command                  | Description                   |
| ------------------------ | ----------------------------- |
| SET key value EX seconds | Set value with expiry         |
| EXPIRE key seconds       | Add expiry to an existing key |
| TTL key                  | View remaining lifetime       |

### Additional Capabilities

* O(1) average-time key lookup
* O(1) LRU cache maintenance
* Automatic LRU eviction
* Background TTL cleanup
* Snapshot persistence
* Automatic recovery on startup
* Concurrent client handling
* Thread-safe operations using mutexes

---

## Performance Benchmark

Benchmarks were executed on the in-memory database layer using **1,000,000 operations**.

| Operation |         Throughput |
| --------- | -----------------: |
| SET       |   ~709,940 ops/sec |
| GET       |   ~700,699 ops/sec |
| EXISTS    | ~1,118,951 ops/sec |

### Benchmark Configuration

* 1,000,000 operations
* C++17
* STL-based implementation
* O(1) average-time hash table operations
* LRU maintenance enabled
* Mutex-protected thread-safe operations

---

## Architecture

```text
                    TCP Clients
                         |
        ----------------------------------
        |                |               |
        v                v               v

    Client Thread    Client Thread   Client Thread
           \              |              /
            \             |             /
             \            |            /
                Shared Database
                        |
      ---------------------------------------
      |                  |                 |
      v                  v                 v

  Key-Value Store      TTL            LRU Cache
                         |
                    Persistence
```

---

## Data Structures Used

| Component         | Data Structure |
| ----------------- | -------------- |
| Key-Value Storage | unordered_map  |
| LRU Tracking      | list           |
| Expiry Tracking   | unordered_map  |
| Synchronization   | mutex          |

---

## Project Structure

```text
redis-clone/
│
├── database.h
├── database.cpp
├── server.cpp
├── benchmark.cpp
├── README.md
└── .gitignore
```

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

## Connect To Server

Using netcat:

```bash
nc localhost 6379
```

---

## Example Usage

### Basic Operations

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
```

### Expiration

```text
SET temp 123 EX 10
OK

TTL temp
10
```

After expiration:

```text
GET temp
NULL
```

### FLUSHALL

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
* Performance Benchmarking

---

## Tech Stack

* C++17
* STL
* POSIX Sockets
* Multithreading
* File I/O
* Linux/macOS Networking APIs

---

## Key Learning Outcomes

This project explores the core concepts behind modern in-memory databases such as Redis, including:

* Efficient data structure design
* Cache eviction strategies
* Expiration management
* Concurrent server architecture
* Persistent storage and recovery
* Network programming using sockets
* Performance measurement and optimization
