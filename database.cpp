#include "database.h"

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