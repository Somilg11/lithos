#include "../include/memtable.h"
#include <random>

MemTable::MemTable() {
    MAX_LEVEL = 4; // Start small for testing
    PROBABILITY = 0.5;
    current_level = 0;
    // Head node acts as the dummy start point
    head = new Node("", "", MAX_LEVEL);
    node_count = 0;
}

MemTable::~MemTable() {
    Node* curr = head;
    while (curr != nullptr) {
        Node* next = curr->forward[0];
        delete curr;
        curr = next;
    }
}

int MemTable::random_level() {
    int lvl = 1;
    // Keep flipping a coin (50% chance)
    while ((static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) < PROBABILITY && lvl < MAX_LEVEL) {
        lvl++;
    }
    return lvl;
}

void MemTable::put(const std::string& key, const std::string& value) {
    std::vector<Node*> update(MAX_LEVEL, nullptr);
    Node* curr = head;

    // 1. Find the insertion point
    for (int i = current_level; i >= 0; i--) {
        while (curr->forward[i] != nullptr && curr->forward[i]->key < key) {
            curr = curr->forward[i];
        }
        update[i] = curr;
    }

    curr = curr->forward[0];

    // 2. If key exists, just update value
    if (curr != nullptr && curr->key == key) {
        curr->value = value;
        return;
    }

    // 3. Otherwise, insert new node
    int new_level = random_level();

    // If new level is higher than current max, update the head pointers
    if (new_level > current_level) {
        for (int i = current_level + 1; i < new_level; i++) {
            update[i] = head;
        }
        current_level = new_level - 1; // 0-indexed
    }

    Node* new_node = new Node(key, value, new_level);

    // 4. Splice the new node into the list
    for (int i = 0; i < new_level; i++) {
        new_node->forward[i] = update[i]->forward[i];
        update[i]->forward[i] = new_node;
    }
    node_count++;
}

bool MemTable::get(const std::string& key, std::string& value_out) {
    Node* curr = head;
    
    // Start from the top fast-lane and drop down
    for (int i = current_level; i >= 0; i--) {
        while (curr->forward[i] != nullptr && curr->forward[i]->key < key) {
            curr = curr->forward[i];
        }
    }
    
    curr = curr->forward[0];
    
    if (curr != nullptr && curr->key == key) {
        value_out = curr->value;
        return true;
    }
    return false;
}

void MemTable::display() {
    std::cout << "\n--- MemTable (Skip List) ---\n";
    for (int i = current_level; i >= 0; i--) {
        Node* curr = head->forward[i];
        std::cout << "Level " << i << ": ";
        while (curr != nullptr) {
            std::cout << "[" << curr->key << ":" << curr->value << "] -> ";
            curr = curr->forward[i];
        }
        std::cout << "NULL\n";
    }
    std::cout << "----------------------------\n";
}

size_t MemTable::size() const {
    return node_count;
}

void MemTable::clear() {
    Node* curr = head->forward[0];
    while (curr != nullptr) {
        Node* next = curr->forward[0];
        delete curr;
        curr = next;
    }
    
    // Reset head forward pointers
    for (int i = 0; i < MAX_LEVEL; i++) {
        head->forward[i] = nullptr;
    }
    
    current_level = 0;
    node_count = 0;
}