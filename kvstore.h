#ifndef KVSTORE_H
#define KVSTORE_H

#include <string>
#include <unordered_map>
#include <fstream>

class KVStore
{
private:
    std::unordered_map<std::string, std::string> store;
    std::ofstream logFile;
    std::string logFileName;

public:
    KVStore(const std::string &file);
    ~KVStore();

    bool put(const std::string &key, const std::string &value);
    bool get(const std::string &key, std::string &out);
    bool del(const std::string &key);
};

#endif // KVSTORE_H

