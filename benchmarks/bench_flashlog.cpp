#include <benchmark/benchmark.h>
#include "logger.hpp"
#include "sinks/console_sink.hpp"
#include "sinks/file_sink.hpp"
#include "ring_buffer.hpp"

// Benchmark 1
static void BM_ringPush(benchmark::State& state) {
    auto rbuff = std::make_unique<flashlog::MPSCRingBuffer<65536>>();
    flashlog::LogRecord rec = flashlog::make_record(flashlog::LogLevel::INFO, "bench msg");
    
    // Background consumer — keeps draining so buffer never fills
    std::atomic<bool> running{true};
    std::thread consumer([&](){
        flashlog::LogRecord tmp;
        while(running.load(std::memory_order_relaxed)) {
            if(!rbuff->pop(tmp)) {
                std::this_thread::yield();
            }
        }
    });

    for (auto _ : state) {
        rbuff->push(rec);
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(state.iterations());

    running.store(false);
    consumer.join();
}
BENCHMARK(BM_ringPush);

// Benchmark 2
static void BM_fullHotPath(benchmark::State& state) {
    flashlog::MPSCRingBuffer<4096> rbuff;
    for (auto _ : state) {
        auto rec = flashlog::make_record(flashlog::LogLevel::INFO, "bench msg");
        rbuff.push(rec);
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_fullHotPath);

// Benchmark 3
static void BM_disabledLevel(benchmark::State& state) {
    for (auto _ : state) {
        LOG_TRACE("this should be nearly free");
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_disabledLevel);

// Benchmark 4 — static so it's in BSS (global), not stack
static void BM_multiThread(benchmark::State& state) {
    static flashlog::MPSCRingBuffer<4096> rbuff;
    flashlog::LogRecord rec = flashlog::make_record(flashlog::LogLevel::INFO, "mt msg");
    for (auto _ : state) {
        rbuff.push(rec);
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_multiThread)->Threads(1)->Threads(2)->Threads(4)->Threads(8);

// Benchmark 5
static void BM_pushPop(benchmark::State& state) {
    flashlog::MPSCRingBuffer<4096> rbuff;
    flashlog::LogRecord rec = flashlog::make_record(flashlog::LogLevel::INFO, "bench msg");
    flashlog::LogRecord tmp;
    for (auto _ : state) {
        rbuff.push(rec);
        rbuff.pop(tmp);
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_pushPop);

BENCHMARK_MAIN();