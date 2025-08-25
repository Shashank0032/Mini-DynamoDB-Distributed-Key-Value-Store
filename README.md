# Mini DynamoDB (C++ Implementation)

This project is a **simplified implementation of DynamoDB** built in **C++**, inspired by concepts of **partitioning, replication, and leader-based consistency**.  
It simulates how DynamoDB partitions data and replicates it across nodes for **fault tolerance** and **high availability**.

---

## 🚀 Features

- **Range-based Partitioning**: Keys are mapped to partitions using a simple range-based hash.
- **Replication**: Each partition is replicated across multiple nodes for reliability.
- **Leader + Replica Model**:
  - One replica per partition acts as the **Leader** (handles writes).
  - Other replicas act as **Followers** (store copies).
- **Key-Value Store API**:
  - `put(key, value)` → Store a value.
  - `get(key)` → Retrieve a value.
  - `showCluster()` → View partition distribution and replication.

---

## 🛠️ Tech Stack

- **Language**: C++17
- **Core Data Structures**:
  - `unordered_map` for key-value storage
  - `vector` for partition and replica management

---

## 📖 Example Usage

```cpp
int main() {
    Cluster cluster(3, 2); // 3 nodes, 2 replicas per partition

    cluster.put("name", "Alice");
    cluster.put("age", "23");
    cluster.put("city", "Hyderabad");

    std::cout << "Get name: " << cluster.get("name") << std::endl;
    std::cout << "Get age: " << cluster.get("age") << std::endl;

    cluster.showCluster();
}
```

✅ Sample Output
Get name: Alice
Get age: 23

---

--- Cluster State ---
Node 0 has partitions: [P1 (Leader), P2 (Replica)]
Node 1 has partitions: [P2 (Leader), P3 (Replica)]
Node 2 has partitions: [P3 (Leader), P1 (Replica)]

💡 Future Improvements

Add failure handling & recovery (simulate node crashes).

Implement consistent hashing for more realistic partitioning.

Add a frontend (React/Node.js) to visualize partition distribution.

Support range queries and secondary indexes.

🎯 Why this Project?

This is a learning-focused mini-project that demonstrates how modern distributed databases (like DynamoDB, Cassandra, etc.) manage:

Partitioning

Replication

Leader-based writes

Fault tolerance

It’s not production-grade but is an excellent educational tool for understanding distributed systems.

## 🏃 How to Run

1.  **Navigate to the project directory**:

    ```bash
    cd mini_dynamo_db
    ```

2.  **Compile the project**:

    ```bash
    g++ -std=c++17 main.cpp cluster.cpp partition.cpp kvstore.cpp -o mini_dynamo_db
    ```

3.  **Run the executable**:

    ```bash
    ./mini_dynamo_db
    ```

    This will start the Mini-DynamoDB CLI, where you can interact with the simulated database.

🧑‍💻 Author

Shashank

GitHub: Shashank0032
