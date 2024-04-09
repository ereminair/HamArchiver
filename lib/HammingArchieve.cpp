#include "HammingArchieve.h"
#include "BitsInput.h"
#include "BitsOutput.h"
#include <cstdlib>

#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

std::vector<std::string> split(const std::string& s, char delimiter) {
    std::vector<std::string> split_bytes;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        split_bytes.push_back(token);
    }
    return split_bytes;
}

std::vector<bool> EncodeHamming(const std::vector<bool>& bin_code, size_t count_of_bytes) {
    size_t word_size = 8 * count_of_bytes;
    auto hamming_bytes = static_cast<size_t>(trunc(log2(static_cast<double>(word_size))) + 1);
    std::vector<bool> encoded_number(word_size + hamming_bytes + 1);
    uint64_t sum_control_bit[word_size + hamming_bytes + 1];
    uint64_t degree_2 = 0;
    sum_control_bit[0] = 0;
    size_t control_bit = 0;

    for (uint64_t i = 1; i <= word_size + hamming_bytes; ++i) {
        sum_control_bit[i] = sum_control_bit[i - 1];
        if (i == (1 << degree_2)) {
            degree_2++;
        } else {
            encoded_number[i] = bin_code[control_bit];
            control_bit++;
            sum_control_bit[i] += encoded_number[i];
        }
    }
    degree_2 = 1;
    uint64_t sum_inform_bit;
    for (uint64_t i = 1; i <= word_size + hamming_bytes; i = (1 << degree_2)) {
        sum_inform_bit = 0;
        for (uint64_t j = i; j <= word_size + hamming_bytes; j += i * 2) {
            size_t min_ = 0;
            if(j + i - 1 < word_size + hamming_bytes) {
                 min_ = j + i - 1;
            } else {
                 min_ = word_size + hamming_bytes;
            }
            sum_inform_bit += sum_control_bit[min_] - sum_control_bit[j - 1];
        }
        encoded_number[i] = sum_inform_bit % 2;
        degree_2++;
    }

    encoded_number = std::vector<bool>(encoded_number.begin() + 1, encoded_number.end());
    return encoded_number;
}

uint64_t FindMistake(const std::vector<bool>& code) {
    uint64_t mistake = 0;

    for (uint64_t i = 0; i < code.size(); ++i) {
        if (code[i]) {
            mistake ^= i + 1;
        }
    }

    return mistake;
}

std::vector<bool> DecodeHamming(const std::vector<bool>& code) {
    std::vector<bool> new_code = code;
    std::vector<bool> result;
    uint64_t mistake = FindMistake(new_code);
    if (mistake != 0 && mistake < new_code.size()) {
        new_code[mistake - 1] = !new_code[mistake - 1];
        mistake = FindMistake(new_code);
        if (mistake) {
            throw std::invalid_argument("Too many mistakes or incorrect code");
        }
    }

    uint64_t degree_2 = 1;
    for (uint64_t i = 0; i < new_code.size(); i++)
        if (i + 1 == degree_2 && i != new_code.size() - 1) {
            degree_2 *= 2;
        } else {
            result.push_back(new_code[i]);
        }

    return result;
}

void HammingArchieve::CreateArch(const std::string& name, const std::vector<std::string>& filenames) const {
    std::string file_name_header = "HAF|";
    
    int sum_file = 0;
    std::vector<size_t> size_files;
    for (const auto& filename : filenames) {

        file_name_header += filename + '|';
        std::ifstream fin(filename, std::ios::binary);
        file_name_header += std::to_string(sum_file) + '|';
        sum_file += fin.seekg(0, std::ios::end).tellg();
        size_files.push_back(fin.tellg());
    }
    file_name_header += '!';

    BitsOutput bitsout(name);
    for (char c : file_name_header) {
        if (c != '\0') {
            std::bitset<8> bits = c;
            std::vector<bool> new_code;

            for (int i = 7; i >= 0; --i) {
                new_code.push_back(bits[i]);
            }
            bitsout.WriteBits(EncodeHamming(new_code, count_of_bytes));
        }
    }
    for (int i = 0; i < filenames.size(); ++i) {
        BitsInput bitinput(filenames[i]);

        std::vector<bool> bits;
        int count = 0;
        while (bitinput.ReadBits(8 * count_of_bytes, bits)) {
            count++;
            bitsout.WriteBits(EncodeHamming(bits, count_of_bytes));
            if (count == size_files[i]) {
                break;
            }
        }
    }
}

void HammingArchieve::AddFileToArch(const std::string& name, const std::vector<std::string>& filenames) const {
    std::string old_file_name_header;
    std::vector<std::vector<bool>> decode_info;
    std::vector<bool> bits;
    bool now_file_name_header = true;
    BitsInput bitinputt(name);
    while (bitinputt.ReadBits(8 * count_of_bytes + static_cast<size_t>(trunc(log2(static_cast<double>(8 * count_of_bytes))) + 1), bits)) {
        bits = DecodeHamming(bits);
        if (!now_file_name_header) {
            decode_info.push_back(bits);
            continue;
        }

        char c;
        for (int i = 0; i < 8; ++i) {
            c = static_cast<char>((c << 1) + bits[i]);
        }
        if (c == '!') {
            now_file_name_header = false;
            continue;
        }

        if (now_file_name_header) {
            old_file_name_header += c;
            continue;
        }
    }
    std::ifstream fin(name);
    std::ifstream fin2(filenames[0]);
    size_t size_file = fin2.seekg(0, std::ios::end).tellg();
    fin.seekg(0, std::ios::end);


    std::string new_file_name_header = old_file_name_header;
    new_file_name_header += filenames[0] + '|';
    std::vector<std::string> file_name_header_parts = split(old_file_name_header, '|');

    size_t size = ((((fin.tellg() * 8) / 12) - std::stoi(file_name_header_parts[file_name_header_parts.size() - 1]) - old_file_name_header.size() - 1))
                  + std::stoi(file_name_header_parts[file_name_header_parts.size() - 1]);
    new_file_name_header += std::to_string(size) + "|!";

    BitsOutput bitsout(name);
    for (char c : new_file_name_header) {
        if (c != '\0') {
            std::bitset<8> to_bits = c;
            std::vector<bool> new_code;

            for (int i = 7; i >= 0; --i) {
                new_code.push_back(to_bits[i]);
            }

            bitsout.WriteBits(EncodeHamming(new_code, count_of_bytes));
        }
    }

    for (auto& vec : decode_info) {
        bitsout.WriteBits(EncodeHamming(vec, count_of_bytes));
    }

    size_t count = 0;
    BitsInput bitinputt2(filenames[0]);
    while (bitinputt2.ReadBits(8 * count_of_bytes, bits)) {
        count++;
        bitsout.WriteBits(EncodeHamming(bits, count_of_bytes));
        if (count == size_file) {
            break;
        }
    }
}

void HammingArchieve::List(const std::string& name) const {
    std::ifstream fin(name, std::ios::binary);
    std::string file_name_header;
    std::vector<bool> bits;
    BitsInput bitinputt(name);
    while (bitinputt.ReadBits(8 * count_of_bytes + static_cast<size_t>(trunc(log2(static_cast<double>(8 * count_of_bytes))) + 1), bits)) {
        bits = DecodeHamming(bits);
        char output_char;
        for (int i = 0; i < 8; i++) {
            output_char = static_cast<char>((output_char << 1) + bits[i]);
        }

        if (output_char == '!') {
            break;
        }

        file_name_header += output_char;
    }
    std::vector<std::string> file_name_header_split = split(file_name_header, '|');
    std::vector<std::string> filenames_;
    for (int i = 1; i < file_name_header_split.size(); i += 2) {
        filenames_.push_back(file_name_header_split[i]);
    }
    for (const auto& filename : filenames_) {
        std::cout << filename << '\n';
    }
}

void HammingArchieve::ExtractArch(const std::string& name, const std::vector<std::string>& filenames) const {
    std::string file_name_header;
    std::string content;
    std::vector<bool> bits;
    BitsInput bitinputt(name);
    size_t file_name_header_size = 0;
    while (bitinputt.ReadBits(8 * count_of_bytes + static_cast<size_t>(trunc(log2(static_cast<double>(8 * count_of_bytes))) + 1), bits)) {
        bits = DecodeHamming(bits);
        char output_char;
        for (int i = 0; i < 8; i++) {
            output_char = static_cast<char>((output_char << 1) + bits[i]);
        }

        if (output_char == '!') {
            break;
        }

        file_name_header += output_char;
    }
    std::vector<std::string> file_name_header_split = split(file_name_header, '|');
    std::vector<std::string> filenames_separately;
    std::vector<size_t> sum_files2;
    for (int i = 1; i < file_name_header_split.size(); i += 2) {
        filenames_separately.push_back(file_name_header_split[i]);
    }
    for (int i = 2; i < file_name_header_split.size(); i += 2) {
        sum_files2.push_back(std::stoi(file_name_header_split[i]));
    }

    std::vector<std::pair<std::string, size_t>> files_to_extract;
    if (filenames.empty()) {
        for (int i = 0; i < filenames_separately.size(); ++i) {
            files_to_extract.emplace_back(filenames_separately[i], sum_files2[i]);
        }
    } else {
        for (int i = 0; i < filenames_separately.size(); ++i) {
            for (const auto& filename : filenames) {
                if (filenames_separately[i] == filename) {
                    files_to_extract.emplace_back(filenames_separately[i], sum_files2[i]);
                }
            }
        }
    }

    size_t cur_pos = 0;
    for (int i = 0; i < files_to_extract.size(); ++i) {
        std::string prefix = name.substr(0, name.find('.')) + "_";
        BitsOutput bitsout(prefix + files_to_extract[i].first);
        if (files_to_extract.size() > 1) {
            size_t cur_prefsum = files_to_extract[i].second;
            size_t next_prefsum = files_to_extract[i + 1].second;
            while (bitinputt.ReadBits(8 * count_of_bytes + static_cast<size_t>(trunc(log2(static_cast<double>(8 * count_of_bytes))) + 1), bits)) {
                if (cur_pos < cur_prefsum) {
                    cur_pos++;
                    continue;
                } else {
                    bits = DecodeHamming(bits);
                    bitsout.WriteBits(bits);
                    cur_pos++;
                    if(cur_pos == next_prefsum){
                        break;
                    }
                }
            }
        } else {
            while (bitinputt.ReadBits( 8*count_of_bytes + static_cast<size_t>(trunc(log2(static_cast<double>(8 * count_of_bytes))) +1), bits)) {
                if (cur_pos < files_to_extract[i].second) {
                    cur_pos++;
                    continue;
                } else {
                    bits = DecodeHamming(bits);
                    bitsout.WriteBits(bits);
                    cur_pos++;
                }
            }
        }
    }
}

void HammingArchieve::DelFileToArch(const std::string& name, const std::vector<std::string>& filenames) const {
    std::string old_file_name_header;
    std::vector<std::vector<bool>> decode_info;
    std::vector<bool> bits;
    bool now_file_name_header = true;
    BitsInput bitinputt(name);
    while (bitinputt.ReadBits(8 * count_of_bytes + static_cast<size_t>(trunc(log2(static_cast<double>(8 * count_of_bytes))) + 1), bits)) {
        bits = DecodeHamming(bits);

        if (!now_file_name_header) {
            decode_info.push_back(bits);
            continue;
        }

        char c;
        for (int i = 0; i < 8; ++i) {
            c = static_cast<char>((c << 1) + bits[i]);
        }

        if (c == '!') {
            now_file_name_header = false;
            continue;
        }

        if (now_file_name_header) {
            old_file_name_header += c;
            continue;
        }
    }
    std::vector<std::string> file_name_header_parts = split(old_file_name_header, '|');
    std::vector<std::string> filenames_separately;
    std::vector<size_t> file_sizes;
    std::vector<size_t> sum_separately;
    for (int i = 1; i < file_name_header_parts.size(); i += 2) {
        filenames_separately.push_back(file_name_header_parts[i]);
    }
    for (int i = 2; i < file_name_header_parts.size(); i += 2) {
        sum_separately.push_back(std::stoi(file_name_header_parts[i]));
    }
    for (int i = 0; i < filenames_separately.size(); ++i) {
        if (i == filenames_separately.size() - 1) {
            file_sizes.push_back(decode_info.size() - sum_separately[i]);
        } else {
            file_sizes.push_back(sum_separately[i + 1] - sum_separately[i]);
        }
    }

    std::vector<std::string> new_filenames;
    std::vector<size_t> new_file_sizes;
    std::vector<size_t> new_sum_files2(1, 0);

    int encoded_number_of_del_file = -1;
    for (int i = 0; i < filenames_separately.size(); ++i) {
        if (filenames[0] != filenames_separately[i]) {

            new_filenames.push_back(filenames_separately[i]);
            new_file_sizes.push_back(file_sizes[i]);
        } else {
            encoded_number_of_del_file = i;
        }
    }
    for (int i = 0; i < file_sizes.size(); ++i) {
        new_sum_files2.push_back(new_sum_files2[i] + new_file_sizes[i]);
    }

    std::string new_file_name_header = "HAF|";
    for (int i = 0; i < new_filenames.size(); ++i) {
        new_file_name_header += new_filenames[i] + "|" + std::to_string(new_sum_files2[i]) + "|";
    }
    new_file_name_header += "!";
    std::cout << new_file_name_header << '\n';
    if (encoded_number_of_del_file == -1) {
        std::cout << "File not found" << std::endl;
        return;
    }

    BitsOutput bitsout(name);
    for (char c : new_file_name_header) {
        if (c != '\0') {
            std::bitset<8> to_bits = c;
            std::vector<bool> new_code;

            for (int i = 7; i >= 0; --i) {
                new_code.push_back(to_bits[i]);
            }

            bitsout.WriteBits(EncodeHamming(new_code, count_of_bytes));
        }
    }

    size_t cur_pos = 0;
    size_t start = sum_separately[encoded_number_of_del_file];
    size_t end = (encoded_number_of_del_file == filenames_separately.size() - 1) ? decode_info.size() : sum_separately[encoded_number_of_del_file + 1];
    for (const std::vector<bool>& bits_ : decode_info) {
        if (!(cur_pos >= start && cur_pos < end)) {
            bitsout.WriteBits(EncodeHamming(bits_, count_of_bytes));
        }
        cur_pos++;
    }
}

void HammingArchieve::ContagenateArchs(const std::string& name, const std::vector<std::string>& filenames) const {
    std::string old_file_name_header1;
    std::vector<std::vector<bool>> decode_info1;
    std::vector<bool> bits1;
    bool now_file_name_header = true;
    BitsInput bitinputt(filenames[0]);
    int count = 0;
    int posfile_name_header1 = 0;
    while (bitinputt.ReadBits(8 * count_of_bytes + static_cast<size_t>(trunc(log2(static_cast<double>(8 * count_of_bytes))) + 1), bits1)) {
        bits1 = DecodeHamming(bits1);

        if (!now_file_name_header) {
            decode_info1.push_back(bits1);
            continue;
        }

        char c;
        for (int i = 0; i < 8; ++i) {
            c = static_cast<char>((c << 1) + bits1[i]);
        }

        if (c == '!') {
            now_file_name_header = false;
            continue;
        }

        if (now_file_name_header) {
            old_file_name_header1 += c;
            count++;
            continue;
        }
    }
    posfile_name_header1 = count;
    std::vector<std::string> file_name_header_parts = split(old_file_name_header1, '|');
    std::vector<std::string> filenames_separately1;
    std::vector<size_t> sum_separately1;
    for (int i = 1; i < file_name_header_parts.size(); i += 2) {
        filenames_separately1.push_back(file_name_header_parts[i]);
    }
    for (int i = 2; i < file_name_header_parts.size(); i += 2) {
        sum_separately1.push_back(std::stoi(file_name_header_parts[i]));
    }
    //std::cout << decode_info1.size() << ' ' << posfile_name_header1 << ' ' << sum_separately1[sum_separately1.size() - 1] << std::endl;
    std::string new_file_name_header1 = "HAF|";
    for (int i = 0; i < filenames_separately1.size(); ++i) {
        new_file_name_header1 += filenames_separately1[i] + "|" + std::to_string(sum_separately1[i]) + "|";
    }
    std::string old_file_name_header2;
    std::vector<std::vector<bool>> decode_info2;
    std::vector<bool> bits2;
    bool now_file_name_header2 = true;
    BitsInput bitinputt2(filenames[1]);
    while (bitinputt2.ReadBits(8 * count_of_bytes + static_cast<size_t>(trunc(log2(static_cast<double>(8 * count_of_bytes))) + 1), bits2)) {
        bits2 = DecodeHamming(bits2);

        if (!now_file_name_header2) {
            decode_info2.push_back(bits2);
            continue;
        }

        char c;
        for (int i = 0; i < 8; ++i) {
            c = static_cast<char>((c << 1) + bits2[i]);
        }

        if (c == '!') {
            now_file_name_header2 = false;
            continue;
        }

        if (now_file_name_header2) {
            old_file_name_header2 += c;
            continue;
        }
    }
    std::vector<std::string> file_name_header_parts2 = split(old_file_name_header2, '|');
    std::vector<std::string> filenames_separately2;
    std::vector<size_t> sum_separately2;
    for (int i = 1; i < file_name_header_parts2.size(); i += 2) {
        filenames_separately2.push_back(file_name_header_parts2[i]);
    }
    for (int i = 2; i < file_name_header_parts2.size(); i += 2) {
        sum_separately2.push_back(std::stoi(file_name_header_parts2[i]));
    }
    for (unsigned long long & i : sum_separately2) {
        i += decode_info1.size();
    }
    std::string new_file_name_header2;
    for (int i = 0; i < filenames_separately2.size(); ++i) {
        new_file_name_header2 += filenames_separately2[i] + "|" + std::to_string(sum_separately2[i]) + "|";
    }
    std::string new_file_name_header = new_file_name_header1 + new_file_name_header2 + '!';
    std::cout << new_file_name_header << std::endl;
    BitsOutput bitsout(name);
    for (char c : new_file_name_header) {
        if (c != '\0') {
            std::bitset<8> to_bits = c;
            std::vector<bool> new_code;

            for (int i = 7; i >= 0; --i) {
                new_code.push_back(to_bits[i]);
            }

            bitsout.WriteBits(EncodeHamming(new_code, count_of_bytes));
        }
    }
    for (const std::vector<bool>& bits_ : decode_info1) {
        bitsout.WriteBits(EncodeHamming(bits_, count_of_bytes));
    }
    for (const std::vector<bool>& bits_ : decode_info2) {
        bitsout.WriteBits(EncodeHamming(bits_, count_of_bytes));
    }
}


