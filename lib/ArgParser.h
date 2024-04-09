#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <cstdint>

class ArgParser {
    public:
    explicit ArgParser(const std::vector<std::string>& input) {
        ArgParser::input = input;
    }
    bool Parse();
    [[nodiscard]] bool GetCreate() const {
        return have_create;
    }
    [[nodiscard]] bool GetList() const {
        return have_list;
    }
    [[nodiscard]] bool GetExtract() const {
        return have_extract;
    }
    [[nodiscard]] bool GetAppend() const {
        return have_append;
    }
    [[nodiscard]] bool GetDelete() const {
        return have_delete;
    }
    [[nodiscard]] bool GetConcatenate() const {
        return have_concatenate;
    }
    [[nodiscard]] std::string GetFilename() const {
        return filename_;
    }
    [[nodiscard]] size_t GetCountOfBytes() const {
        return count_of_bytes;
    }
    [[nodiscard]] std::vector<std::string> GetFilenames() const {
        return filenames;
    }
    [[nodiscard]] bool GetRead() const {
        return have_reading;
    }

private:
    bool have_create = false;
    bool have_list = false;
    bool have_extract = false;
    bool have_append = false;
    bool have_delete = false;
    bool have_concatenate = false;
    size_t count_of_bytes = 1;
    bool have_reading = false;
    std::string filename_ = "";
    std::vector <std::string> input;
    std::vector <std::string> filenames;
};