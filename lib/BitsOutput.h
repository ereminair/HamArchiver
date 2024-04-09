#pragma once

#include <fstream>
#include <vector>
#include <bitset>
#include <iostream>
#include <cstdint>

static const uint16_t kBufferOutSize = 64;

class BitsOutput {
public:
    ~BitsOutput();
    bool WriteBits(const std::vector<bool>& bits);
    explicit BitsOutput(const std::string& output_file);
private:
    std::ofstream file;
    bool waiting_bits[kBufferOutSize]{};
    uint16_t waiting_bits_size = 0;

};