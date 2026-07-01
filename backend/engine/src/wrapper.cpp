#include "../include/wrapper.h"
#include <string>
#include <filesystem> 
#include <fstream>    

// Initialize BloomFilter (e.g., 10000 bits, 3 hashes) and counters
Database::Database() : wal("lithos.wal"), bloom(10000, 3), total_reads(0), saved_disk_reads(0) {}

void Database::put(rust::Str key, rust::Str value) {
    std::string k(key.data(), key.size());
    std::string v(value.data(), value.size());
    
    wal.append(k, v);
    wal.sync();
    mem.put(k, v);
    
    // <-- ADDED: Teach the bloom filter about this key
    bloom.add(k);

    if (mem.size() >= 5) {
        flush();
    }
}

rust::String Database::get(rust::Str key) {
    std::string k(key.data(), key.size());
    std::string out;
    
    // <-- ADDED: Track every query attempt
    total_reads++; 
    
    // 1. FAST PATH: Check RAM
    if (mem.get(k, out)) {
        return rust::String(out); 
    }

    // <-- ADDED: BLOOM FILTER INTERCEPT
    // If the filter guarantees the key isn't here, skip the disk scan!
    if (!bloom.possibly_contains(k)) {
        saved_disk_reads++; 
        return rust::String(""); 
    }

    // 2. SLOW PATH: Check Disk
    try {
        for (const auto& entry : std::filesystem::directory_iterator(".")) {
            if (entry.is_regular_file() && entry.path().filename().string().find(".db") != std::string::npos) {
                
                std::ifstream file(entry.path());
                std::string line;
                
                while (std::getline(file, line)) {
                    auto delim = line.find(':');
                    if (delim != std::string::npos) {
                        std::string file_key = line.substr(0, delim);
                        if (file_key == k) {
                            return rust::String(line.substr(delim + 1));
                        }
                    }
                }
            }
        }
    } catch (...) {
        // Fail gracefully on file locks
    }

    return rust::String(""); 
}

std::unique_ptr<Database> new_database() {
    return std::make_unique<Database>();
}

size_t Database::get_memtable_size() {
    return mem.size(); 
}

int Database::get_level_count(int level) {
    int count = 0;
    try {
        for (const auto& entry : std::filesystem::directory_iterator(".")) {
            if (entry.is_regular_file()) {
                std::string name = entry.path().filename().string();
                
                if (name.find(".db") != std::string::npos) {
                    if (level == 0) {
                        if (name.find("sstable_") == 0 && name.find("_L") == std::string::npos) {
                            count++;
                        }
                    } else {
                        std::string level_marker = "sstable_L" + std::to_string(level) + "_";
                        if (name.find(level_marker) == 0) {
                            count++;
                        }
                    }
                }
            }
        }
    } catch (...) {
        return 0; 
    }
    return count;
}

void Database::flush() {
    int next_id = get_level_count(0); 
    std::string filename = "sstable_" + std::to_string(next_id) + ".db";

    std::ofstream out(filename);
    
    Node* curr = mem.get_head()->forward[0];
    while (curr != nullptr) {
        out << curr->key << ":" << curr->value << "\n";
        curr = curr->forward[0];
    }
    out.close();

    mem.clear();
}

// <-- ADDED: Implementation for the telemetry getters
size_t Database::get_total_reads() { return total_reads; }
size_t Database::get_saved_reads() { return saved_disk_reads; }