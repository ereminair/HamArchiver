#include "BitsInput.h"

bool BitsInput::NextBit() {
    if (waiting_bits_size == 0) {
        if (file.eof()) {
            throw std::out_of_range("End of file");
        }

        char c = file.get();
        std::bitset<8> symbol_bits = c;

        for (int i = waiting_bits_size; i < waiting_bits_size + 8; ++i) {
            waiting_bits[i] = symbol_bits[8 - (i - waiting_bits_size) - 1];
        }

        waiting_bits_size += 8;

        bool first_bit = waiting_bits[0];
        for (int i = 0; i < waiting_bits_size - 1; ++i) {
            waiting_bits[i] = waiting_bits[i + 1];
        }

        waiting_bits_size--;

        return first_bit;
    } else {
        bool first_bit = waiting_bits[0];
        for (int i = 0; i < waiting_bits_size - 1; ++i) {
             waiting_bits[i] = waiting_bits[i + 1];
        }

        waiting_bits_size--;

        for (int i = waiting_bits_size; i < kBufferInSize; ++i) {
            waiting_bits[i] = false;
        }

        return first_bit;
    }
}

bool BitsInput::ReadBits(uint64_t count, std::vector<bool>& bits) {
    try {
        std::vector<bool> answer;
        for (int i = 0; i < count; i++) {
            answer.push_back(NextBit());
        }
        bits = answer;
        return true;
    } catch (std::out_of_range& e) {
        return false;
    }
}

BitsInput::BitsInput(const std::string& input_file)  {
    file.open(input_file, std::ios::binary);

      for (bool& waiting_bit : waiting_bits) {
          waiting_bit = false;
      }
      if (!file.is_open()) {
          throw std::invalid_argument("Can't open this file: " + input_file);
      }
}

void BitsInput::ClearWaitingBits() {
      for (bool& waiting_bit : waiting_bits) {
          waiting_bit = false;
      }

      waiting_bits_size = 0;
}

void BitsInput::ReadBinary(const std::string& inputFilename, const std::string& outputFilename) {
    std::ifstream inputFile(inputFilename, std::ios::binary);

    if (!inputFile.is_open()) {
        std::cerr << "Unable to open input file: " << inputFilename << std::endl;
        return;
    }

    std::ofstream outputFile(outputFilename, std::ios::binary);

    if (!outputFile.is_open()) {
        std::cerr << "Unable to open output file: " << outputFilename << std::endl;
        return;
    }

    char byte;
    while (inputFile.read(&byte, 1)) {
        std::bitset<8> bits(byte);
        outputFile.write(bits.to_string().c_str(), 8);
    }

    inputFile.close();
    outputFile.close();
}

BitsInput::~BitsInput() {
      file.close();
      ClearWaitingBits();
}
