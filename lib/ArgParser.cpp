#include "ArgParser.h"

bool ArgParser::Parse() {
    for (int i = 0; i < input.size(); ++i) {
        if (input[i] == "-c" || input[i] == "--create") {
            have_create = true;
            continue;
        }
        if (input[i] == "-f") {
            filename_ = input[i + 1];
            i++;
            continue;
        }
        if (input[i].substr(0, 7) == "--file=") {
            filename_ = input[i].substr(7, input[i].size() - 7);
            continue;
        }
        if (input[i] == "-r" || input[i] == "--reading") {
            have_reading = true;
            continue;
        }
        if (input[i] == "-l" || input[i] == "--list") {
            have_list = true;
            continue;
        }
        if (input[i] == "-x" || input[i] == "--extract") {
            have_extract = true;
            continue;
        }
        if (input[i] == "-a" || input[i] == "--append") {
            have_append = true;
            continue;
        }
        if (input[i] == "-d" || input[i] == "--delete") {
            have_delete = true;
            continue;
        }
        if (input[i] == "-A" || input[i] == "--concatenate") {
            have_concatenate = true;
            continue;
        }
        if (input[i][0] != '-') {
          filenames.push_back(input[i]);
          continue;
        }
    }
    if (have_create + have_list + have_extract + have_append + have_delete + have_concatenate + have_reading != 1) {
        return false;
    } else {
        if (have_create) {
            if (filename_.empty() || filenames.empty()) {
                return false;
            }
        }
        if (have_reading) {
            if (filename_.empty()) {
                return false;
            }
        }
        if (have_list || have_extract) {
            if (filename_.empty()) {
                return false;
            }
        }
        if (have_append || have_delete) {
            if (filename_.empty() || filenames.size() != 1) {
                return false;
            }
        }
        if (have_concatenate) {
            if (filenames.size() != 2 || filename_.empty()) {
                return false;
            }
        }

    return true;
  }
}

