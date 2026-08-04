// Copyright (c) 2026 Vinod Parmar
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once
#include <string_view>
#include "level.hpp"

namespace flashlog {
class ISink {
public:
    virtual void write(std::string_view formatted_line, LogLevel level) = 0;
    virtual void flush() = 0;
    virtual ~ISink() = default;
};

}