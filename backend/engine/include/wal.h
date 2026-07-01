#ifndef WAL_H
#define WAL_H

#include <string>
#include <fstream>
#include <iostream>

class WAL {
private:
    std::ofstream log_file;
    std::string file_path;

public:
    WAL(const std::string& path);
    ~WAL();
    
    // Appends a key-value pair to the log
    bool append(const std::string& key, const std::string& value);
    
    // Forces the OS to flush the buffer to physical disk
    void sync();
};

#endif