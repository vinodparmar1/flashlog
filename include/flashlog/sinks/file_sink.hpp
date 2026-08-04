// Copyright (c) 2026 Vinod Parmar
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <cstdio>
#include <string>
#include <stdexcept>
#include "sink.hpp"

namespace flashlog {
class FileSink : public ISink {
private:
    FILE* fbuff;
public:
    FileSink(const std::string& file) {
        fbuff = fopen(file.c_str(), "a");
        if(fbuff == nullptr) {
            throw std::runtime_error("Failed to open log file: " + file);
        }
    }
    virtual void write(std::string_view formatted_line, [[maybe_unused]] LogLevel level) override {
        fwrite(formatted_line.data(), sizeof(char), formatted_line.size(), fbuff);
        fwrite("\n", sizeof(char), 1, fbuff);
    }
    virtual void flush() override {
        fflush(fbuff);
    }
    ~FileSink() {
        if(fbuff) {
            fflush(fbuff);
            fclose(fbuff);
            fbuff = nullptr;
        }
    }
    FileSink(const FileSink& ) = delete; // no copy op
    FileSink(FileSink&&) = delete; // no move op
    FileSink& operator=(const FileSink& ) = delete; // no copy assignment op
};

}
