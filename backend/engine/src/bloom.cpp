#include "../include/bloom.h"

BloomFilter::BloomFilter(size_t size, int num_hashes) 
    : size(size), num_hashes(num_hashes) {
    bit_array.resize(size, false);
}

uint32_t BloomFilter::hash(const std::string& key, uint32_t seed) const {
    uint32_t h = 0x811c9dc5 ^ seed; // FNV offset basis
    for (char c : key) {
        h ^= static_cast<uint32_t>(c);
        h *= 0x01000193; // FNV prime
    }
    return h;
}

void BloomFilter::add(const std::string& key) {
    for (int i = 0; i < num_hashes; i++) {
        uint32_t h = hash(key, i); // Use 'i' as the seed to get different hashes
        bit_array[h % size] = true;
    }
}

bool BloomFilter::possibly_contains(const std::string& key) const {
    for (int i = 0; i < num_hashes; i++) {
        uint32_t h = hash(key, i);
        if (!bit_array[h % size]) {
            return false; // Definitely not present
        }
    }
    return true; // Probably present
}