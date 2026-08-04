// Copyright (c) 2026 Vinod Parmar
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once
#include <thread>
#include <vector>
#include <chrono>
#include "ring_buffer.hpp"
#include "sink.hpp"
#include "formatter.hpp"

namespace flashlog {

template <size_t BufferCapacity = 8192>
class AsyncWorker {
private:
    MPSCRingBuffer<BufferCapacity>& rbuff_;
    Formatter& fmtr_;
    std::vector<ISink*>& sinks_;
    std::atomic<bool> running_;
    std::thread worker_;

    void write_record(const LogRecord& rec) {
        std::string line;
        fmtr_.format(rec, line);
        for(size_t i = 0; i < sinks_.size(); ++i) {
            sinks_[i]->write(line, rec.level);
        }
    }

    void drain_loop() {
        while(running_.load(std::memory_order_relaxed)) {
            LogRecord rec;
            if(rbuff_.pop(rec)) {
                write_record(rec);
            }
            else {
                // should be configurable
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }
        // drain after stop
        LogRecord rec;
        while(rbuff_.pop(rec)) {
            write_record(rec);
        }
        // flush all sinks on stop
        for(auto* sink : sinks_) {
            sink->flush();
        }
    }

public:
    AsyncWorker(MPSCRingBuffer<BufferCapacity>& rbuff,
                Formatter& fmtr,
                std::vector<ISink*>& sinks):
                rbuff_(rbuff), fmtr_(fmtr), sinks_(sinks), running_(false) {
        
    }

    void start() {
        if(!running_.load(std::memory_order_acquire)) {
            running_.store(true, std::memory_order_release);
            worker_ = std::thread(&AsyncWorker::drain_loop, this);
        }
    }
    void stop() {
        if(running_.load(std::memory_order_acquire) && worker_.joinable()) {
            running_.store(false, std::memory_order_release);
            worker_.join();
        }
    }

    AsyncWorker(const AsyncWorker&) = delete;
    AsyncWorker& operator=(const AsyncWorker&) = delete;

    ~AsyncWorker() {
        stop();
    }
};
}



