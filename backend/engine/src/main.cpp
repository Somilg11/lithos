#include "../include/wal.h"
#include "../include/memtable.h"
#include "../include/sstable.h"
#include "../include/bloom.h"
#include <iostream>
#include <vector>

int main() {
    std::cout << "Booting Lithos Compaction Worker...\n\n";

    // Simulate Time Period 1
    MemTable mem1;
    mem1.put("01_alice", "admin");
    mem1.put("05_eve", "user");
    SSTable::flush(mem1, "sstable_0.db");

    // Simulate Time Period 2
    MemTable mem2;
    mem2.put("02_bob", "guest");
    mem2.put("06_frank", "user");
    SSTable::flush(mem2, "sstable_1.db");

    // Simulate Time Period 3
    MemTable mem3;
    mem3.put("03_charlie", "admin");
    mem3.put("04_dave", "guest");
    SSTable::flush(mem3, "sstable_2.db");

    std::cout << "\n--- Triggering Background Compaction ---\n";
    
    // Group the files to merge
    std::vector<std::string> files_to_merge = {
        "sstable_0.db", 
        "sstable_1.db", 
        "sstable_2.db"
    };

    // Compact them into a Level 1 file
    SSTable::compact(files_to_merge, "sstable_L1_0.db");

    // Verify the merged output
    std::cout << "\nVerifying compacted file (sstable_L1_0.db)...\n";
    std::string result;
    if (SSTable::get("sstable_L1_0.db", "04_dave", result)) {
        std::cout << "Disk Read Success: 04_dave -> " << result << "\n";
    }

    return 0;
}