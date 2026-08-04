// Copyright (c) 2026 Vinod Parmar
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <format>
#include <ctime>
#include <cstdio>
#include "record.hpp"

namespace flashlog {

class Formatter {
public:
    void format(const LogRecord& rec, std::string& out) const {
        out.clear();
        std::format_to(std::back_inserter(out),
                        "{} [{}] [0x{:x}] {} {} {}",
                        format_timestamp(rec.timestamp_ns),
                        to_string(rec.level),
                        rec.thread_id,
                        format_location(rec.location),
                        std::string_view(rec.fields.data(), rec.fields_len),
                        std::string_view(rec.message.data(), rec.msg_len));
    }

private:
    std::string format_timestamp(uint64_t timestamp_ns) const {
        auto tp = std::chrono::system_clock::time_point(
                    std::chrono::nanoseconds(timestamp_ns));
        auto secs = std::chrono::duration_cast<std::chrono::seconds>(tp.time_since_epoch());
        std::time_t t = secs.count();
        std::tm tm_info;
        gmtime_r(&t, &tm_info);
        char date_buf[32];
        strftime(date_buf, sizeof(date_buf), "%Y-%m-%d %H:%M:%S", &tm_info);
        auto us = std::chrono::duration_cast<std::chrono::microseconds>(
                  tp.time_since_epoch()) % 1'000'000;
        char full_buf[40];
        std::snprintf(full_buf, sizeof(full_buf), "%s.%06lld", date_buf, (long long)us.count());
        return full_buf;
    }

    std::string format_location(const std::source_location& loc) const {
        std::string str_loc;
        std::string_view fname = loc.file_name();
        auto pos = fname.find_last_of('/');
        fname = (pos != std::string_view::npos) ? fname.substr(pos+1) : fname;
        return std::format("{}:{}",
                    fname,
                    loc.line());
    }
};

}
