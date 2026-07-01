#pragma once
#include "memtable.h"
#include "wal.h"
#include "bloom.h" // <-- ADDED
#include "rust/cxx.h"
#include <memory>

class Database {
public:
    Database();
    void put(rust::Str key, rust::Str value);
    rust::String get(rust::Str key);

    size_t get_memtable_size();
    int get_level_count(int level);
    void flush();
    
    // <-- ADDED TELEMETRY GETTERS
    size_t get_total_reads();
    size_t get_saved_reads();

private:
    MemTable mem;
    WAL wal;
    BloomFilter bloom; // <-- ADDED
    
    // <-- ADDED COUNTERS
    size_t total_reads;
    size_t saved_disk_reads;
};

std::unique_ptr<Database> new_database();