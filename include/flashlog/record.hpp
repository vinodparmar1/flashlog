// Copyright (c) 2026 Vinod Parmar
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <array>
#include <chrono>
#include <thread>
#include <source_location>
#include <cstring>
#include "level.hpp"

namespace flashlog {
struct LogRecord {
    // 8-byte fields together (no padding needed between them)
    uint64_t timestamp_ns;          // 8 bytes
    size_t msg_len;                 // 8 bytes
    size_t fields_len;              // 8 bytes

    // 4-byte fields together (no padding between them)
    LogLevel level;                 // 4 bytes (enum class underlying type)
    uint32_t thread_id;             // 4 bytes
    
    // large buffers
    std::array<char, 256> message;  // 256 bytes
    std::array<char, 256> fields;   // 256 bytes

    // variable size
    std::source_location location;  // varies
};

LogRecord make_record(LogLevel lev, 
                      std::string_view msg,
                      std::source_location loc = std::source_location::current()) {
    LogRecord rec;
    auto now = std::chrono::system_clock::now();
    rec.timestamp_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
    rec.level = lev;
    // std::hash<std::thread::id> converts any thread::id to a size_t
    // consistently on any platform, then we truncate to uint32_t:
    rec.thread_id = static_cast<uint32_t>(std::hash<std::thread::id>{}(std::this_thread::get_id()));
    rec.location = loc;
    size_t copy_len = std::min(msg.size(), rec.message.size());
    std::memcpy(rec.message.data(), msg.data(), copy_len);
    rec.msg_len = copy_len;
    rec.fields_len = 0;
    return rec;
}

}

// Step by step:
/*
std::hash<std::thread::id> hasher;          // create hash functor
size_t hash_val = hasher(this_thread::get_id()); // call operator()
uint32_t tid = static_cast<uint32_t>(hash_val);  // truncate to 32 bits
*/
