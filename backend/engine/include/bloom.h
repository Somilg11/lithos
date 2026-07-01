#ifndef BLOOM_H
#define BLOOM_H

#include <string>
#include <vector>
#include <cstdint>

class BloomFilter {
private:
    std::vector<bool> bit_array;
    int num_hashes;
    size_t size;

    // A simple, fast hash function (FNV-1a variant)
    uint32_t hash(const std::string& key, uint32_t seed) const;

public:
    BloomFilter(size_t size, int num_hashes);
    
    // Add a key to the filter
    void add(const std::string& key);
    
    // Check if a key might exist
    bool possibly_contains(const std::string& key) const;
};

#endif