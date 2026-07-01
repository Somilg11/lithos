# Bloom Filter

## What is it?
A Bloom Filter is a highly space-efficient probabilistic data structure kept entirely in RAM. It is used to test whether an element is a member of a set. It can tell the database two things:
1. "The key is **definitely not** in the database."
2. "The key is **probably** in the database."

## Why is it implemented?
Reading from a physical disk (SSTables) is the most expensive operation in a database. If a user queries a key that doesn't exist, the engine would normally have to open the file, search it, and only then realize the key is missing. This wastes massive amounts of I/O bandwidth.

By checking the in-memory Bloom Filter first, the engine can instantly abort the search if the filter guarantees the key doesn't exist, bypassing the disk entirely.

## How it works (The Data Flow)
1. **Initialization:** The filter is an array of bits (e.g., 100 bits) all initialized to `0`.
2. **Populating (During Flush):** When an SSTable is written to disk, every key is hashed using multiple distinct hash functions (e.g., 3 functions). Each hash outputs an integer index. The bits at those specific indices in the array are flipped to `1`.
3. **Querying (During Read):** When a client requests a key, the engine hashes that key using the *same* 3 hash functions.
    * If **any** of the bits at those indices are `0`, the key was never added. (Definite No).
    * If **all** of the bits at those indices are `1`, the key *might* exist. (Probable Yes). The engine then proceeds to search the disk.

## How it helps (False Positives vs. False Negatives)
* **Zero False Negatives:** A Bloom Filter will *never* tell you a key doesn't exist if it actually does. Data is never lost or skipped.
* **Acceptable False Positives:** Occasionally, the hash functions for a new key will land on bits that were already flipped to `1` by completely different keys. The filter will say "Probably Yes," and the engine will check the disk and find nothing. This is an acceptable trade-off because it still prevents 95%+ of unnecessary disk reads.

## Implementation Details (C++ Core)
In Lithos, the Bloom Filter is implemented using `std::vector<bool>` and the **FNV-1a** hash algorithm.
* **FNV-1a:** Chosen because it is incredibly fast to compute (unlike cryptographic hashes like SHA-256) and provides excellent uniform distribution of bits, minimizing collisions.
* **Seed Variation:** Instead of writing three entirely different hash algorithms, Lithos passes an integer `seed` (0, 1, 2) into the single FNV-1a function to generate three distinct hash outputs for the same key.