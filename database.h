#ifndef DATABASE_H
#define DATABASE_H

#include <unordered_map>
#include <string>
#include <ctime>
#include <thread>
#include <mutex>
#include <chrono>

class Database {
private:

    std::unordered_map<
        std::string,
        std::string
    > store;

    std::unordered_map<
        std::string,
        time_t
    > expiry;

    std::mutex mtx;

    bool isExpired(
        const std::string& key
    );

public:

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

    bool exists(
        const std::string& key
    );

    int size();
};

#endif