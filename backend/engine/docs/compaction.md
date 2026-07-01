# Background Compaction (K-Way Merge)

## What is it?
Compaction is a background maintenance process that takes multiple smaller, older SSTable files from the disk, merges them together, removes duplicate or deleted keys, and writes a single, larger, highly optimized SSTable file.

## Why is it implemented?
As an LSM tree runs, it continuously flushes MemTables to disk, creating dozens or hundreds of Level 0 SSTables.
1. **Read Amplification:** If a key isn't caught by the Bloom filter, the engine has to open and search *every single* Level 0 file to find it. This destroys read performance.
2. **Space Amplification:** If a user updates their profile picture URL 5 times, the database stores all 5 versions in 5 different SSTables. Only the newest one matters; the other 4 are wasting physical disk space.

Compaction solves both problems by consolidating files and discarding stale data.

## How it works (The Data Flow)
1. **Trigger:** A background thread wakes up when too many Level 0 files accumulate (e.g., 4 files).
2. **Open Streams:** The engine opens file reading streams (`std::ifstream`) for all target files simultaneously.
3. **Initialize Min-Heap:** The engine reads the very first key-value pair from each file and pushes them into a Min-Heap (`std::priority_queue`). Because the files are already sorted internally, the absolute smallest key across *all* files is guaranteed to be at the top of the heap.
4. **Merge & Write:** * The engine pops the smallest key from the heap and writes it to the new `Level_1.db` file.
    * If the same key is popped again (meaning it existed in multiple files), the engine keeps the newest one and discards the old one (Deduplication).
    * The engine looks at which file the popped key came from, reads the *next* line from that specific file, and pushes it into the heap.
5. **Clean Up:** Once the heap is empty, the new Level 1 file is committed to disk, and the old Level 0 files are permanently deleted.

## Implementation Details (C++ Core)
In Lithos, compaction utilizes standard competitive programming data structures applied to file I/O.
* **`std::priority_queue`:** By default, C++ creates a Max-Heap. Lithos overrides the comparator to use `std::greater`, creating a Min-Heap based on lexicographical string sorting.
* **Low Memory Footprint:** The algorithm is strictly $O(N \log K)$ time complexity (where $N$ is total keys and $K$ is the number of files) and $O(K)$ space complexity. Even if you are merging 100 Gigabytes of files, Lithos only ever holds exactly $K$ lines in RAM at any given time.