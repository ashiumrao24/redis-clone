#include "database.h"

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
){
    std::lock_guard<std::mutex>
        lock(mtx);
    store[key] = value;
    expiry.erase(key);
}

void Database::setWithExpiry(
    const std::string& key,
    const std::string& value,
    int seconds
){
    std::lock_guard<std::mutex>
        lock(mtx);
    store[key] = value;
    expiry[key] =
        time(nullptr) + seconds;
}

std::string Database::get(
    const std::string& key
){
    std::lock_guard<std::mutex> lock(mtx);
    if(isExpired(key)){
        store.erase(key);
        expiry.erase(key);
        return "NULL";
    }
    auto it = store.find(key);
    if(it == store.end())
        return "NULL";
    return it->second;
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
                    store.erase(it->first);
                    it = expiry.erase(it);
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
    store.erase(key);
    expiry.erase(key);
}

bool Database::exists(
    const std::string& key
){
    std::lock_guard<std::mutex> lock(mtx);
    if(isExpired(key)){
        store.erase(key);
        expiry.erase(key);
        return false;
    }
    return store.find(key)!= store.end();
}

int Database::size(){
    std::lock_guard<std::mutex> lock(mtx);
    return store.size();
}