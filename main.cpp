#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include "cluster.h"

// helper: tokenize
static std::vector<std::string> splitTokens(const std::string &line)
{
    std::vector<std::string> tokens;
    std::istringstream iss(line);
    std::string tok;
    while (iss >> tok)
        tokens.push_back(tok);
    return tokens;
}

int main()
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    Cluster cluster(3, 2); // 3 nodes, 2 replicas per partition

    std::cout << "\nMini-DynamoDB (Partitioned) CLI\n";
    std::cout << "Commands:\n  PUT <Key> <Value>\n  GET <Key>\n  DEL <Key>\n  PLACE <Key>\n  PARTITIONS\n  NODES\n  DOWN <Node>\n  UP <Node>\n  SHOWCLUSTER\n  Q\n\n";

    std::string line;
    while (true)
    {
        std::cout << "> " << std::flush;
        if (!std::getline(std::cin, line))
            break;
        if (line.empty())
            continue;

        auto tokens = splitTokens(line);
        if (tokens.empty())
            continue;

        std::string cmd = tokens[0];
        std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);

        if (cmd == "Q")
        {
            std::cout << "Bye\n";
            break;
        }

        if (cmd == "PUT")
        {
            if (tokens.size() < 3)
            {
                std::cout << "(error) PUT requires <Key> <Value>\n";
                continue;
            }
            std::string k = tokens[1], v = tokens[2];
            cluster.debugKeyPlacement(k);
            bool ok = cluster.put(k, v);
            std::cout << (ok ? "PUT OK\n" : "PUT FAILED\n");
        }
        else if (cmd == "GET")
        {
            if (tokens.size() < 2)
            {
                std::cout << "(error) GET requires <Key>\n";
                continue;
            }
            std::string k = tokens[1], out;
            cluster.debugKeyPlacement(k);
            if (cluster.get(k, out))
                std::cout << k << " -> " << out << "\n";
            else
                std::cout << k << " -> [NOT FOUND]\n";
        }
        else if (cmd == "DEL")
        {
            if (tokens.size() < 2)
            {
                std::cout << "(error) DEL requires <Key>\n";
                continue;
            }
            std::string k = tokens[1];
            cluster.debugKeyPlacement(k);
            bool ok = cluster.del(k);
            std::cout << (ok ? "DEL OK\n" : "DEL FAILED\n");
        }
        else if (cmd == "PLACE")
        {
            if (tokens.size() < 2)
            {
                std::cout << "(error) PLACE requires <Key>\n";
                continue;
            }
            cluster.debugKeyPlacement(tokens[1]);
        }
        else if (cmd == "PARTITIONS")
        {
            cluster.listPartitions();
        }
        else if (cmd == "NODES")
        {
            cluster.listNodes();
        }
        else if (cmd == "DOWN")
        {
            if (tokens.size() < 2)
            {
                std::cout << "(error) DOWN requires <Node>\n";
                continue;
            }
            cluster.setNodeUp(tokens[1], false);
        }
        else if (cmd == "UP")
        {
            if (tokens.size() < 2)
            {
                std::cout << "(error) UP requires <Node>\n";
                continue;
            }
            cluster.setNodeUp(tokens[1], true);
        }
        else if (cmd == "SHOWCLUSTER")
        {
            cluster.showCluster();
        }
        else
        {
            std::cout << "(error) Unknown command\n";
        }
    }
    return 0;
}


