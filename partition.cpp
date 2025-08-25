#include "partition.h"
#include <functional>
#include <algorithm>

Partition::Partition(int numPartitions) : numPartitions(numPartitions) {}

int Partition::partitionForKey(const std::string &key)
{
    if (numPartitions <= 0)
        return -1;
    size_t H = hsh(key);
    unsigned long long maxHash = std::numeric_limits<size_t>::max();
    unsigned long long bucket = (maxHash / (unsigned long long)numPartitions) + 1ULL;
    int part = (int)std::min<unsigned long long>((unsigned long long)H / bucket, (unsigned long long)numPartitions - 1ULL);
    return part;
}


