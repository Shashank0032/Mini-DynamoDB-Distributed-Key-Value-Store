#ifndef PARTITION_H
#define PARTITION_H

#include <string>
#include <vector>
#include <unordered_map>
#include <limits>

// Forward declaration
class KVStore;

class Partition
{
private:
    int numPartitions;

    static size_t hsh(const std::string &s) { return std::hash<std::string>{}(s); }

public:
    Partition(int numPartitions);
    int partitionForKey(const std::string &key);
};

#endif // PARTITION_H

