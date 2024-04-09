#pragma once

#include <fstream>
#include <vector>
#include <bitset>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <cstdint>

const uint16_t kBufferInSize = 64;

class BitsInput {
public:

    void ClearWaitingBits();
    bool ReadBits(uint64_t count, std::vector<bool>& bits);
    explicit BitsInput(const std::string& input_file);
    BitsInput() = default;
    void ReadBinary(const std::string& inputFilename, const std::string& outputFilename);

    ~BitsInput();
private:
    std::ifstream file;
    bool waiting_bits[kBufferInSize] {};
    uint16_t waiting_bits_size = 0;

    bool NextBit();
};
