// Copyright (c) 2026 Vinod Parmar
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <iostream>
#include "sink.hpp"

namespace flashlog {

class ConsoleSink : public ISink {
public:
    ConsoleSink() {}
    virtual void write(std::string_view formatted_line, LogLevel level) override {
        if(level == LogLevel::ERROR || level == LogLevel::FATAL) {
            std::cerr << "\033[31m" << formatted_line << "\n\033[0m";
        }
        else {
            std::cout << "\033[90m" << formatted_line << "\n\033[0m";
        }
    }

    virtual void flush() override {
        // na
    }
    ~ConsoleSink() {
        //na
    }
    ConsoleSink(const ConsoleSink& ) = delete; // no copy op
    ConsoleSink(ConsoleSink&&) = delete; // no move op
    ConsoleSink& operator=(const ConsoleSink& ) = delete; // no copy assignment op
};
}