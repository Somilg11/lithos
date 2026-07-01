#ifndef MEMTABLE_H
#define MEMTABLE_H

#include <string>
#include <vector>
#include <iostream>

struct Node {
    std::string key;
    std::string value;
    std::vector<Node*> forward;

    Node(std::string k, std::string v, int level) 
        : key(k), value(v), forward(level, nullptr) {}
};

class MemTable {
private:
    int MAX_LEVEL;
    float PROBABILITY;
    int current_level;
    Node* head;
    size_t node_count;
    int random_level();

public:
    MemTable();
    ~MemTable();

    void put(const std::string& key, const std::string& value);
    bool get(const std::string& key, std::string& value_out);
    void display();
    
    // NEW: Exposes the head so SSTable can read Level 0 for flushing
    Node* get_head() const { return head; }
    size_t size() const;
    void clear();
};

#endif