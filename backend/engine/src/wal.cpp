#include "../include/wal.h"

WAL::WAL(const std::string& path) : file_path(path) {
    // Open file in append mode. Create it if it doesn't exist.
    log_file.open(file_path, std::ios::app | std::ios::out);
    if (!log_file.is_open()) {
        std::cerr << "Failed to open WAL file: " << file_path << "\n";
    }
}

WAL::~WAL() {
    if (log_file.is_open()) {
        log_file.close();
    }
}

bool WAL::append(const std::string& key, const std::string& value) {
    if (!log_file.is_open()) return false;
    
    // Simple serialization: key,value\n
    log_file << key << "," << value << "\n";
    return true;
}

void WAL::sync() {
    if (log_file.is_open()) {
        log_file.flush(); // Push from RAM buffer to OS disk queue
    }
}