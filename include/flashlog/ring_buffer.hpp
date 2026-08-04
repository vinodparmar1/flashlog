// Copyright (c) 2026 Vinod Parmar
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <atomic>
#include <array>
#include "record.hpp"

namespace flashlog {

template <size_t Capacity>
class MPSCRingBuffer {
static_assert((Capacity &(Capacity -1)) == 0, "Capacity should be power of 2");
public:
    MPSCRingBuffer() {
        for(uint64_t  i =0; i< Capacity; ++i) {
            buffer_[i].sequence.store(i, std::memory_order_relaxed);
        }
    }
    bool push(const LogRecord& rec) {
        auto current_tail = tail_.load(std::memory_order_relaxed);
        while(true) {
            auto slot = &buffer_[current_tail & mask_];
            auto seq = slot->sequence.load(std::memory_order_acquire);
            if(seq == current_tail) {
                if(tail_.compare_exchange_weak(current_tail, current_tail +1,
                std::memory_order_relaxed)) {
                    slot->data = rec;
                    slot->sequence.store(seq+1, std::memory_order_release);
                    return true;
                }
                continue;
            }
            else if(seq < current_tail) {
                return false;
            }
            else {
                continue;
            }
        }
    }

    bool pop(LogRecord& rec) {
        auto slot = &buffer_[head_ & mask_];
        auto seq = slot->sequence.load(std::memory_order_acquire);
        if(seq == head_ +1) {
            rec = slot->data;
            slot->sequence.store(head_ + Capacity, std::memory_order_release);
            ++head_;
            return true;
        }
        else if (seq == head_){
            return false; // empty slot, not produced yet
        }
        return false;
    }
private:
    struct Slot {
        std::atomic<uint64_t> sequence;
        LogRecord data;
    };

    static constexpr uint64_t mask_ = Capacity - 1;
    std::array<Slot, Capacity> buffer_;
    alignas(64) std::atomic<uint64_t> tail_{0}; // producers
    alignas(64) uint64_t head_{0}; // cosumers
};
}

