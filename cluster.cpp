#include "cluster.h"
#include <iostream>
#include <algorithm>
#include <iomanip>

Cluster::Cluster(int numNodes, int rf) : numPartitions(0), replicationFactor(rf), partitionManager(0)
{
    for (int i = 0; i < numNodes; ++i)
    {
        addNode("Node" + std::to_string(i));
    }
    initPartitions(numNodes * 2); // Default to 2 partitions per node
}

void Cluster::addNode(const std::string &node)
{
    if (nodeStores.count(node))
    {
        std::cout << "[Warn] Node " << node << " already exists\n";
        return;
    }
    nodes.push_back(node);
    nodeStores[node] = std::make_unique<KVStore>(node + ".log");
    nodeUp[node] = true;
    std::cout << "[Cluster] Added node " << node << "\n";
}

void Cluster::initPartitions(int P)
{
    if (nodes.empty())
    {
        std::cout << "[Error] No nodes present. Add nodes before initializing partitions.\n";
        return;
    }
    numPartitions = std::max(1, P);
    partitionReplicas.clear();
    partitionManager = Partition(numPartitions);

    int n = (int)nodes.size();
    for (int p = 0; p < numPartitions; ++p)
    {
        std::vector<std::string> reps;
        for (int r = 0; r < replicationFactor; ++r)
        {
            int nodeIndex = (p + r) % n;
            reps.push_back(nodes[nodeIndex]);
        }
        partitionReplicas[p] = reps;
    }
    std::cout << "[Cluster] Initialized " << numPartitions << " partitions with RF=" << replicationFactor << "\n";
}

std::vector<std::string> Cluster::replicaSetForKey(const std::string &key)
{
    int pid = partitionManager.partitionForKey(key);
    if (pid < 0)
        return {};
    if (partitionReplicas.count(pid))
        return partitionReplicas[pid];
    return {};
}

bool Cluster::put(const std::string &key, const std::string &value)
{
    auto rs = replicaSetForKey(key);
    if (rs.empty())
    {
        std::cout << "[Error] No replica set available\n";
        return false;
    }
    int rf = (int)rs.size();
    int quorum = rf / 2 + 1;
    int acks = 0;
    for (auto &node : rs)
    {
        if (!nodeUp[node])
        {
            std::cout << "[PUT] " << node << " DOWN; skip\n";
            continue;
        }
        bool ok = nodeStores[node]->put(key, value);
        if (ok)
            acks++;
    }
    bool success = (acks >= quorum);
    std::cout << "[PUT] key=" << key << " part=" << partitionManager.partitionForKey(key)
             << " RF=" << rf << " quorum=" << quorum << " acks=" << acks
             << (success ? " [COMMIT]\n" : " [FAIL]\n");
    return success;
}

bool Cluster::get(const std::string &key, std::string &out)
{
    auto rs = replicaSetForKey(key);
    if (rs.empty())
        return false;
    if (nodeUp[rs[0]])
    {
        if (nodeStores[rs[0]]->get(key, out))
            return true;
    }
    for (size_t i = 1; i < rs.size(); ++i)
    {
        if (!nodeUp[rs[i]])
            continue;
        if (nodeStores[rs[i]]->get(key, out))
            return true;
    }
    return false;
}

bool Cluster::del(const std::string &key)
{
    auto rs = replicaSetForKey(key);
    if (rs.empty())
    {
        std::cout << "[Error] No replica set available\n";
        return false;
    }
    int rf = (int)rs.size();
    int quorum = rf / 2 + 1;
    int acks = 0;
    for (auto &node : rs)
    {
        if (!nodeUp[node])
        {
            std::cout << "[DEL] " << node << " DOWN; skip\n";
            continue;
        }
        bool ok = nodeStores[node]->del(key);
        if (ok)
            acks++;
    }
    bool success = (acks >= quorum);
    std::cout << "[DEL] key=" << key << " part=" << partitionManager.partitionForKey(key)
             << " RF=" << rf << " quorum=" << quorum << " acks=" << acks
             << (success ? " [COMMIT]\n" : " [FAIL]\n");
    return success;
}

void Cluster::debugKeyPlacement(const std::string &key)
{
    int pid = partitionManager.partitionForKey(key);
    auto rs = replicaSetForKey(key);
    std::cout << "[Placement] key=" << key << " -> partition=" << pid << " -> ";
    for (size_t i = 0; i < rs.size(); ++i)
    {
        std::cout << (i == 0 ? "(Leader) " : "(Replica) ") << rs[i] << (i + 1 < rs.size() ? ", " : "");
    }
    std::cout << "\n";
}

void Cluster::listPartitions()
{
    std::cout << "Partitions (partition_id : leader, replicas...)\n";
    for (int p = 0; p < numPartitions; ++p)
    {
        std::cout << std::setw(3) << p << " : ";
        auto &v = partitionReplicas[p];
        for (size_t i = 0; i < v.size(); ++i)
        {
            std::cout << (i == 0 ? "(L)" : "(R)") << v[i] << (i + 1 < v.size() ? ", " : "");
        }
        std::cout << "\n";
    }
}

void Cluster::listNodes()
{
    std::cout << "Nodes:\n";
    for (auto &n : nodes)
    {
        std::cout << "  " << n << " [" << (nodeUp[n] ? "UP" : "DOWN") << "]\n";
    }
}

void Cluster::setNodeUp(const std::string &node, bool up)
{
    if (!nodeUp.count(node))
    {
        std::cout << "[Warn] Unknown node " << node << "\n";
        return;
    }
    nodeUp[node] = up;
    std::cout << "[Cluster] " << node << " is now " << (up ? "UP" : "DOWN") << "\n";
}

void Cluster::showCluster()
{
    std::cout << "\n--- Cluster State ---\n";
    for (const auto& nodeName : nodes)
    {
        std::cout << "Node " << nodeName.back() << " has partitions: ";
        bool firstPartition = true;
        for (int p = 0; p < numPartitions; ++p)
        {
            const auto& replicas = partitionReplicas[p];
            for (size_t i = 0; i < replicas.size(); ++i)
            {
                if (replicas[i] == nodeName)
                {
                    if (!firstPartition)
                    {
                        std::cout << ", ";
                    }
                    std::cout << "P" << (p + 1) << " (" << (i == 0 ? "Leader" : "Replica") << ")";
                    firstPartition = false;
                    break;
                }
            }
        }
        std::cout << "\n";
    }
}


