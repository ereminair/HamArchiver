#include "BitsOutput.h"

bool BitsOutput::WriteBits(const std::vector<bool>& bits) {
    for (bool bit : bits) {
        waiting_bits[waiting_bits_size++] = bit;

        if (waiting_bits_size >= 8) {
            char c;

            for (int i = 0; i < 8; i++) {
                c = (c << 1) + waiting_bits[0];
                for (int j = 0; j < waiting_bits_size - 1; j++) {
                    waiting_bits[j] = waiting_bits[j + 1];
                }
                waiting_bits_size--;
            }

            file << c;
        }
    }

    return true;
}

BitsOutput::~BitsOutput() {
    if (waiting_bits_size > 0) {
        std::bitset<8> write_bits;
        int current_bit = 0;
        char output_char;

        while (waiting_bits_size != 0) {
            write_bits[current_bit++] = waiting_bits[0];
            for (int i = 0; i < waiting_bits_size - 1; i++) {
                waiting_bits[i] = waiting_bits[i + 1];
            }

            waiting_bits_size--;
        }

        for (int i = 0; i < 8; i++) {
            output_char = static_cast<char>((output_char << 1) + write_bits[i]);

        }

        file << output_char;
    }
}

BitsOutput::BitsOutput(const std::string& output_file) {
    file.open(output_file, std::ios::binary);

    for (bool& waiting_bit : waiting_bits) {
        waiting_bit = false;
    }

    if (!file.is_open()) {
        throw std::invalid_argument("Can't open this file: output.txt");
    }
}


