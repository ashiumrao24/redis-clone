#include "database.h"
#include <fstream>

Database::Database(size_t cap)
{
    capacity = cap;
}

void Database::touchKey(
    const std::string& key
)
{
    auto it = store.find(key);
    if(it == store.end())
        return;
    lruList.erase(
        it->second.second
    );
    lruList.push_front(key);
    it->second.second =
        lruList.begin();
}

void Database::evictLRU()
{
    if(lruList.empty()) return;
    std::string victim=lruList.back();
    removeKey(victim);
}

bool Database::isExpired(
    const std::string& key
){
    auto it = expiry.find(key);
    if(it == expiry.end()){
        return false;
    }
    return time(nullptr) >= it->second;
}

void Database::set(
    const std::string& key,
    const std::string& value
)
{
    std::lock_guard<std::mutex> lock(mtx);
    auto it = store.find(key);
    // key already exists
    if(it != store.end())
    {
        it->second.first = value;
        touchKey(key);
        expiry.erase(key);
        return;
    }
    // capacity full
    if(store.size() >= capacity)
    {
        evictLRU();
    }
    lruList.push_front(key);
    store[key] =
    {
        value,
        lruList.begin()
    };
    expiry.erase(key);
}

void Database::setWithExpiry(
    const std::string& key,
    const std::string& value,
    int seconds
)
{
    std::lock_guard<std::mutex> lock(mtx);
    auto it = store.find(key);
    if(it != store.end())
    {
        it->second.first = value;
        touchKey(key);
        expiry[key] =
            time(nullptr) + seconds;
        return;
    }
    if(store.size() >= capacity)
    {
        evictLRU();
    }
    lruList.push_front(key);
    store[key] =
    {
        value,
        lruList.begin()
    };
    expiry[key] =
        time(nullptr) + seconds;
}

std::string Database::get(
    const std::string& key
){
    std::lock_guard<std::mutex> lock(mtx);
    if(isExpired(key)){
        removeKey(key);
        return "NULL";
    }
    auto it = store.find(key);
    if(it == store.end()) return "NULL";
    touchKey(key);
    return it->second.first;
}

void Database::cleanupExpiredKeys()
{
    while(true)
    {
        {
            std::lock_guard<std::mutex>
                lock(mtx);
            auto now = time(nullptr);
            for(auto it = expiry.begin();
                it != expiry.end(); )
            {
                if(now >= it->second)
                {
                    std::string key = it->first;
                    ++it;
                    removeKey(key);
                }
                else
                {
                    ++it;
                }
            }
        }
        std::this_thread::sleep_for(
            std::chrono::seconds(1)
        );
    }
}

std::vector<std::string>
Database::getKeys()
{
    std::lock_guard<std::mutex>
        lock(mtx);

    std::vector<std::string> keys;

    for(const auto& pair : store)
    {
        if(
            !expiry.count(pair.first) ||
            !isExpired(pair.first)
        )
        {
            keys.push_back(
                pair.first
            );
        }
    }

    return keys;
}

void Database::del(
    const std::string& key
){
    std::lock_guard<std::mutex> lock(mtx);
    removeKey(key);
}

bool Database::exists(
    const std::string& key
){
    std::lock_guard<std::mutex> lock(mtx);
    if(isExpired(key)){
        removeKey(key);
        return false;
    }
    return store.find(key)!= store.end();
}

void Database::flushAll()
{
    std::lock_guard<std::mutex>
        lock(mtx);

    store.clear();

    expiry.clear();

    lruList.clear();

}

int Database::size(){
    std::lock_guard<std::mutex> lock(mtx);
    return store.size();
}

void Database::removeKey(
    const std::string& key
)
{
    auto it = store.find(key);
    if(it == store.end()) return;
    lruList.erase(
        it->second.second
    );
    store.erase(it);
    expiry.erase(key);
}

bool Database::expire(
    const std::string& key,
    int seconds
)
{
    std::lock_guard<std::mutex>
        lock(mtx);

    auto it =
        store.find(key);

    if(it == store.end())
    {
        return false;
    }

    expiry[key] =
        time(nullptr) + seconds;

    return true;
}

int Database::ttl(
    const std::string& key
)
{
    std::lock_guard<std::mutex>
        lock(mtx);
    auto it =
        store.find(key);
    if(it == store.end())
    {
        return -2;
    }
    auto expIt =
        expiry.find(key);
    if(expIt == expiry.end())
    {
        return -1;
    }
    if(isExpired(key))
    {
        return -2;
    }
    return static_cast<int>(
        expIt->second -
        time(nullptr)
    );
}

void Database::saveToDisk()
{
    std::lock_guard<std::mutex>
        lock(mtx);
    std::ofstream file(
        "database.txt"
    );
    if(!file.is_open()) return;
    for(const auto& entry : store)
    {
        std::string key = entry.first;
        std::string value = entry.second.first;
        time_t expiryTime = 0;
        auto expIt = expiry.find(key);
        if(expIt != expiry.end())
        {
            expiryTime = expIt->second;
        }

        file<< key<< "|"<< value<< "|"<< expiryTime<< "\n";
    }
    file.close();
}

void Database::loadFromDisk()
{
    std::lock_guard<std::mutex>
        lock(mtx);
    std::ifstream file(
        "database.txt"
    );
    if(!file.is_open()) return;
    std::string line;
    while(std::getline(file, line))
    {
        size_t firstPipe = line.find('|');

        size_t secondPipe = line.find('|',firstPipe + 1);

        if(firstPipe == std::string::npos ||
            secondPipe == std::string::npos)
        {
            continue;
        }

        std::string key = line.substr(0,firstPipe);

        std::string value =line.substr(firstPipe + 1,secondPipe - firstPipe - 1);

        std::string expiryStr = line.substr(secondPipe + 1);

        time_t expiryTime = std::stoll(expiryStr);

        if(
            expiryTime != 0 &&
            expiryTime <= time(nullptr)
        )
        {
            continue;
        }

        lruList.push_front(key);

        store[key] =
        {
            value,
            lruList.begin()
        };
        if(expiryTime != 0)
        {
            expiry[key] =expiryTime;
        }
    }
    file.close();
}