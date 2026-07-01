#include "../include/sstable.h"
#include <fstream>
#include <iostream>
#include <queue>
#include <vector>
#include <string>

// --- 1. Flush: Write MemTable to Disk ---
void SSTable::flush(const MemTable& memtable, const std::string& filepath) {
    std::ofstream outfile(filepath, std::ios::out);
    if (!outfile.is_open()) {
        std::cerr << "Failed to create SSTable: " << filepath << "\n";
        return;
    }

    Node* curr = memtable.get_head()->forward[0];
    int count = 0;

    while (curr != nullptr) {
        outfile << curr->key << "," << curr->value << "\n";
        curr = curr->forward[0];
        count++;
    }

    outfile.close();
    std::cout << "Flushed " << count << " keys to SSTable: " << filepath << "\n";
}

// --- 2. Get: Search Disk for Key ---
bool SSTable::get(const std::string& filepath, const std::string& search_key, std::string& value_out) {
    std::ifstream infile(filepath);
    if (!infile.is_open()) return false;

    std::string line;
    while (std::getline(infile, line)) {
        size_t comma_pos = line.find(',');
        if (comma_pos != std::string::npos) {
            std::string key = line.substr(0, comma_pos);
            
            if (key == search_key) {
                value_out = line.substr(comma_pos + 1);
                return true;
            }
            // EARLY EXIT: Sorted data check
            if (key > search_key) {
                break; 
            }
        }
    }
    return false;
}

// --- 3. Compact: K-Way Merge using Min-Heap ---

// Helper struct for the Min-Heap
struct HeapNode {
    std::string key;
    std::string value;
    size_t file_index;

    // We want a Min-Heap, so we reverse the greater-than operator
    bool operator>(const HeapNode& other) const {
        return key > other.key;
    }
};

void SSTable::compact(const std::vector<std::string>& input_files, const std::string& output_file) {
    std::vector<std::ifstream*> streams;
    std::priority_queue<HeapNode, std::vector<HeapNode>, std::greater<HeapNode>> min_heap;

    // Open all files and read the first line of each into the heap
    for (size_t i = 0; i < input_files.size(); i++) {
        std::ifstream* file = new std::ifstream(input_files[i]);
        if (!file->is_open()) continue;
        
        streams.push_back(file);
        
        std::string line;
        if (std::getline(*file, line)) {
            size_t comma = line.find(',');
            if (comma != std::string::npos) {
                min_heap.push({line.substr(0, comma), line.substr(comma + 1), i});
            }
        }
    }

    std::ofstream outfile(output_file, std::ios::out);
    std::string last_key = "";

    // Process the heap until empty
    while (!min_heap.empty()) {
        HeapNode node = min_heap.top();
        min_heap.pop();

        // Write to output file (deduplicating if same key appears multiple times)
        if (node.key != last_key) {
            outfile << node.key << "," << node.value << "\n";
            last_key = node.key;
        }

        // Read the next line from the file this node came from
        std::string next_line;
        if (std::getline(*(streams[node.file_index]), next_line)) {
            size_t comma = next_line.find(',');
            if (comma != std::string::npos) {
                min_heap.push({next_line.substr(0, comma), next_line.substr(comma + 1), node.file_index});
            }
        }
    }

    outfile.close();

    // Clean up memory
    for (auto file : streams) {
        file->close();
        delete file;
    }

    std::cout << "Compacted " << input_files.size() << " files into " << output_file << "\n";
}