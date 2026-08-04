// Copyright (c) 2026 Vinod Parmar
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <string_view>

namespace flashlog {

enum class LogLevel {
    TRACE,
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL,
    OFF
};

constexpr std::string_view to_string(LogLevel level) {
    switch(level) {
        case LogLevel::TRACE: return "TRACE";
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO: return "INFO ";
        case LogLevel::WARN: return "WARN ";
        case LogLevel::ERROR: return "ERROR";
        case LogLevel::FATAL: return "FATAL";
        case LogLevel::OFF: return "OFF  ";
        default: return "?????";
    }
}

constexpr LogLevel FLASHLOG_MIN_LEVEL = LogLevel::INFO;

constexpr bool is_enabled(LogLevel level) {
    return level >= FLASHLOG_MIN_LEVEL  ;
}

}
