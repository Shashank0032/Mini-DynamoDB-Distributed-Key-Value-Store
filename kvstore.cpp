#include "kvstore.h"
#include <iostream>
#include <sstream>

KVStore::KVStore(const std::string &file) : logFileName(file)
{
    std::ifstream infile(file);
    if (infile.is_open())
    {
        std::string op, key, value;
        while (infile >> op >> key)
        {
            if (op == "PUT")
            {
                infile >> value;
                store[key] = value;
            }
            else if (op == "DEL")
            {
                store.erase(key);
            }
        }
        infile.close();
    }
    logFile.open(file, std::ios::app);
}

KVStore::~KVStore()
{
    if (logFile.is_open())
        logFile.close();
}

bool KVStore::put(const std::string &key, const std::string &value)
{
    store[key] = value;
    if (!logFile.is_open())
        return false;
    logFile << "PUT " << key << " " << value << "\n";
    logFile.flush();
    return true;
}

bool KVStore::get(const std::string &key, std::string &out)
{
    auto it = store.find(key);
    if (it == store.end())
        return false;
    out = it->second;
    return true;
}

bool KVStore::del(const std::string &key)
{
    store.erase(key);
    if (!logFile.is_open())
        return false;
    logFile << "DEL " << key << "\n";
    logFile.flush();
    return true;
}


