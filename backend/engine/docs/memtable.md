# MemTable (Memory Table)

## What is it?
The MemTable is the primary in-memory data structure of a Log-Structured Merge (LSM) tree. It temporarily stores all recent, incoming key-value pairs in Random Access Memory (RAM). In Lithos, it is implemented as a **Skip List**.

## Why is it implemented?
Writing data directly to a sorted file on a hard drive for every single database operation is catastrophically slow (known as Write Amplification). The MemTable solves this by acting as a high-speed buffer. 
1. **Instant Sorting:** It automatically sorts incoming keys lexicographically in memory.
2. **Read/Write Speed:** It provides $O(\log N)$ time complexity for both searches and insertions.
3. **Batching:** It allows the engine to accumulate a large batch of sorted data in RAM, which can later be written to disk in one massive, lightning-fast sequential write (an SSTable).

## How it works (The Data Flow)
1. **Insert Request:** After the operation is safely appended to the WAL, the key-value pair is passed to the MemTable.
2. **Coin Flip (Probabilistic Balancing):** The engine "flips a coin" to determine how high the node's "fast-lane" pointers should go. This keeps the tree balanced without the complex rotation algorithms required by Red-Black or AVL trees.
3. **Splice & Route:** The engine routes through the Skip List, dropping down levels until it finds the correct sorted position on Level 0. It then splices the new node into the list, updating the necessary forward pointers.
4. **Read Request:** When a client requests a key, the MemTable is queried first. It scans the highest fast-lane level, drops down when it overshoots the target, and either finds the key or confirms it is not currently in RAM.

## How it helps (Skip List vs. Binary Trees)
While standard databases often use B-Trees or Red-Black trees, high-performance LSM engines (like RocksDB and LevelDB) prefer Skip Lists for the MemTable for two distinct reasons:
* **Concurrency:** It is much easier to implement lock-free, thread-safe concurrent writes on a Skip List than on a strict binary tree. Binary trees require locking massive sections of the tree to perform balancing rotations.
* **Range Queries:** Level 0 of a Skip List is essentially a standard, perfectly sorted Linked List. This makes iterating over a range of keys (e.g., "Give me all users from user_10 to user_50") incredibly fast and simple.

## Implementation Details (C++ Core)
In Lithos, the MemTable relies on standard pointer manipulation and probabilistic mathematics.
* **`Node` Struct:** Contains the `key`, `value`, and a `std::vector<Node*>` acting as an array of forward pointers for different levels.
* **`random_level()`:** Uses `std::rand()` cast strictly to `float` to safely generate a probability curve. Most nodes stay at Level 0, roughly half make it to Level 1, a quarter to Level 2, keeping the overall structure flat and searchable.
* **Level 0 Traversal:** The lowest level (`forward[0]`) represents the complete, sorted dataset, which is the exact structure needed when flushing to an SSTable.