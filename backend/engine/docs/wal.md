# Write-Ahead Log (WAL)

## What is it?
The Write-Ahead Log (WAL) is an append-only, sequential binary or text file stored directly on the physical disk. It acts as the absolute source of truth for all incoming database operations before they are processed by the in-memory engine.

## Why is it implemented?
The primary in-memory data structure of an LSM tree (the MemTable) lives entirely in RAM. RAM is volatile. If the database crashes, the server loses power, or the OS kills the process, all data currently in the MemTable is permanently lost. 

The WAL is implemented to provide **Durability** (the 'D' in ACID properties). By writing to the disk *before* updating RAM, the database guarantees that acknowledged writes can always be recovered upon reboot.

## How it works (The Data Flow)
1. **Receive:** The client sends a `put(key, value)` request.
2. **Log:** The engine immediately appends this operation to the end of the `wal.log` file.
3. **Sync:** The engine flushes the OS buffer to ensure the bytes are physically on the disk platter/NAND flash.
4. **Memory Insert:** Only after the disk write succeeds does the engine insert the data into the in-memory MemTable (Skip List).
5. **Acknowledge:** The engine returns a `200 OK` success message to the client.
6. **Cleanup:** Once the MemTable fills up and flushes to an SSTable, the old WAL file is deleted, and a new one is created.

## How it helps (Performance vs. Durability)
Writing to disk is usually the slowest part of a database. However, the WAL bypasses this bottleneck by relying strictly on **Sequential I/O**.
* **Random Disk Writes:** Jumping around a disk to update specific rows is extremely slow.
* **Sequential Disk Writes:** Appending continuously to the end of a single file is incredibly fast (often matching network speeds), even on older HDDs. 
The WAL gives the database the durability of a disk write, with a latency close to a pure memory write.

## Implementation Details (C++ Core)
In Lithos, the WAL is implemented using the standard C++ `<fstream>` library.
* `std::ios::app`: Opens the file in append mode. This ensures the disk write head stays at the end of the file, maintaining $O(1)$ time complexity for all inserts.
* `std::ofstream::flush()`: Used in the `sync()` method. By default, the operating system caches file writes in memory to optimize disk usage. Calling `flush()` forces the OS to bypass the cache and write directly to the physical storage device, guaranteeing crash safety.