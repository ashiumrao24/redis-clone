#ifndef DATABASE_H
#define DATABASE_H

#include <unordered_map>
#include <string>
#include <ctime>
#include <thread>
#include <mutex>
#include <chrono>
#include <list>

class Database {
private:

    std::unordered_map<
        std::string,
        std::pair<
            std::string,
            std::list<std::string>::iterator
        >
    >store;

    std::list<std::string> lruList;

    size_t capacity;

    std::unordered_map<
        std::string,
        time_t
    > expiry;

    std::mutex mtx;

    bool isExpired(
        const std::string& key
    );

    void touchKey(
        const std::string& key
    );

    void evictLRU();

    void removeKey(
        const std::string& key
    );

public:

    Database(size_t cap = 5);

    void set(
        const std::string& key,
        const std::string& value
    );

    void setWithExpiry(
        const std::string& key,
        const std::string& value,
        int seconds
    );

    std::string get(
        const std::string& key
    );

    void del(
        const std::string& key
    );

    void cleanupExpiredKeys();

    void saveToDisk();

    void loadFromDisk();

    bool exists(
        const std::string& key
    );

    int size();

};

#endif