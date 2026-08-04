// Copyright (c) 2026 Vinod Parmar
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <memory>
#include <vector>
#include "async_worker.hpp"
#include "sink.hpp"
#include "record.hpp"

namespace flashlog {
class Logger {
public:
    static Logger& instance() {
        static Logger logger_;
        return logger_;
    }

    template <typename SinkType, typename... Args>
    void add_sink(Args&&... args) {
        sinks_.push_back(new SinkType(std::forward<Args>(args)...));
    }

    void start() {
        worker_ = std::make_unique<AsyncWorker<1024>>(rlogbuff_, fmtr_, sinks_);
        worker_->start();
    }
    void stop() {
        if(worker_) {
            worker_->stop();
            for(auto* sink: sinks_) {
                sink->flush();
            }
        }
    }
    void log(LogLevel level, std::string_view msg,
             std::source_location sloc= std::source_location::current()) {
        if(!is_enabled(level)) return;
        LogRecord rec = make_record(level, msg, sloc);
        rlogbuff_.push(rec); // push to ring buffer
    }
    ~Logger() {
        stop();
        for(auto* sink: sinks_) {
            if(sink != nullptr) {
                delete sink;
            }
        }
        sinks_.clear();
    }

private:
    std::unique_ptr<AsyncWorker<1024>> worker_;
    std::vector<ISink*> sinks_;
    MPSCRingBuffer<1024> rlogbuff_;
    Formatter fmtr_;
    Logger() = default;
    Logger(const Logger&) = delete;
    Logger(Logger&&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger& operator=(Logger&&) = delete;
};

}

#define LOG_TRACE(msg) \
    do {  if constexpr (flashlog::is_enabled(flashlog::LogLevel::TRACE)) \
        flashlog::Logger::instance().log(flashlog::LogLevel::TRACE, msg); \
    } while(0)
#define LOG_DEBUG(msg) flashlog::Logger::instance().log(flashlog::LogLevel::DEBUG, msg)
#define LOG_INFO(msg) flashlog::Logger::instance().log(flashlog::LogLevel::INFO, msg)
#define LOG_WARN(msg) flashlog::Logger::instance().log(flashlog::LogLevel::WARN, msg)
#define LOG_ERROR(msg) flashlog::Logger::instance().log(flashlog::LogLevel::ERROR, msg)
#define LOG_FATAL(msg) flashlog::Logger::instance().log(flashlog::LogLevel::FATAL, msg)

/*

flashlog/
├── CMakeLists.txt
├── include/
│   └── flashlog/
│       ├── level.hpp
│       ├── record.hpp
│       ├── formatter.hpp
│       ├── sink.hpp
│       ├── console_sink.hpp
│       ├── file_sink.hpp
│       ├── ring_buffer.hpp
│       ├── async_worker.hpp
│       └── logger.hpp
└── src/
    └── main.cpp
*/