# Sorted String Table (SSTable)

## What is it?
An SSTable (Sorted String Table) is an immutable, sequential file stored on disk. It contains a set of key-value pairs that are strictly sorted by the key. Once an SSTable is written to disk, it is never modified—it can only be read or eventually deleted during a background compaction process.

## Why is it implemented?
RAM is a finite resource. The MemTable cannot grow infinitely. When the MemTable hits a predefined memory limit, the engine must clear it out to make room for new incoming writes. 

Because the MemTable (Skip List) is already perfectly sorted in memory, the engine can dump it to an SSTable in one continuous, sequential disk write. This avoids the massive performance penalty of random disk I/O, ensuring that data is persisted safely without blocking the database.

## How it works (The Data Flow)

### 1. The Flush Path (Write)
1. **Freeze:** The active MemTable hits its capacity. It becomes a read-only structure, and a new empty MemTable takes over incoming writes.
2. **Iterate & Write:** A background thread iterates through Level 0 of the frozen MemTable (which acts as a standard sorted linked list) and writes the key-value pairs sequentially to a new `.db` file (e.g., `sstable_001.db`).
3. **Commit:** Once the file is safely on disk, the old MemTable is wiped from RAM, and the corresponding Write-Ahead Log (WAL) is deleted to save space.

### 2. The Search Path (Read)
1. **Query:** A client requests a key (e.g., `get("02_bob")`).
2. **RAM Miss:** The engine checks the active MemTable. If the key is not there, it must search the disk.
3. **Disk Scan:** The engine opens the SSTable file and begins reading.
4. **The Early Exit:** Because the data is strictly sorted, the search algorithm can aggressively optimize. If the engine is looking for `02_bob` and reads a line containing `03_somil`, it knows with absolute certainty that `02_bob` is not in the file. It instantly aborts the search, saving significant I/O time.

## How it helps (Read Optimization & Compaction)
Standard log files require O(N) sequential scanning to find a key. SSTables guarantee fast read performance from the disk because:
* **Lexicographical Ordering:** The strict sorting enables early exits during linear scans and opens the door for Binary Search or Block Sparse Indexing.
* **Easily Compressible:** Sorted data often shares prefixes, making block compression algorithms (like Snappy or LZ4) highly effective for reducing storage costs.
* **Efficient Merging:** Two sorted files can be merged into one perfectly sorted file using a standard K-Way Merge algorithm in O(N) time with minimal RAM overhead.

## Implementation Details (C++ Core)
In Lithos, the SSTable interacts heavily with the OS file system via `<fstream>`.
* **Immutability:** The C++ file stream is opened strictly with `std::ios::out` for creation, and `std::ios::in` for reading. We never append to or update an existing SSTable.
* **Level 0 Extraction:** The `flush` method specifically targets `forward[0]` of the Skip List Node, bypassing the probabilistic fast-lanes to extract the raw, contiguous dataset.