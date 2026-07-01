#ifndef SSTABLE_H
#define SSTABLE_H

#include "memtable.h"
#include <string>
#include <vector>

class SSTable {
public:
    static void flush(const MemTable& memtable, const std::string& filepath);
    static bool get(const std::string& filepath, const std::string& search_key, std::string& value_out);
    
    // Compact multiple Level 0 SSTables into one Level 1 SSTable
    static void compact(const std::vector<std::string>& input_files, const std::string& output_file);
};

#endif