#pragma once

#include <string>
#include <vector>

class HammingArchieve {
public:
    explicit HammingArchieve(size_t count_of_bytes): count_of_bytes(count_of_bytes) {}

    void CreateArch(const std::string& name, const std::vector <std::string>& filenames) const;
    void DelFileToArch(const std::string& name, const std::vector <std::string>& filenames) const;
    void List(const std::string &name) const;
    void AddFileToArch(const std::string& name, const std::vector <std::string>& filenames) const;
    void ExtractArch(const std::string& name, const std::vector <std::string>& filenames) const;
    void ContagenateArchs(const std::string& name, const std::vector <std::string>& filenames) const;
private:
    size_t count_of_bytes = 1;
};

std::vector<bool> EncodeHamming(const std::vector<bool>& bin_code, size_t count_of_bytes);