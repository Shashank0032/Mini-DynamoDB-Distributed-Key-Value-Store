#ifndef CLUSTER_H
#define CLUSTER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include "kvstore.h"
#include "partition.h"

class Cluster
{
private:
    int numPartitions;
    int replicationFactor;
    std::vector<std::string> nodes; // ordered list of nodes
    std::unordered_map<std::string, std::unique_ptr<KVStore>> nodeStores;
    std::unordered_map<std::string, bool> nodeUp;
    std::unordered_map<int, std::vector<std::string>> partitionReplicas;
    Partition partitionManager;

public:
    Cluster(int numNodes, int rf);

    void addNode(const std::string &node);
    void initPartitions(int P);
    std::vector<std::string> replicaSetForKey(const std::string &key);
    bool put(const std::string &key, const std::string &value);
    bool get(const std::string &key, std::string &out);
    bool del(const std::string &key);
    void debugKeyPlacement(const std::string &key);
    void listPartitions();
    void listNodes();
    void setNodeUp(const std::string &node, bool up);
    void showCluster();
};

#endif // CLUSTER_H

