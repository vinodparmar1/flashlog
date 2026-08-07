# flashlog

A fast, lock-free C++ logging library, built to understand how async 
logging works under the hood — MPSC ring buffers, CAS, memory ordering

Header-only. C++20. No dependencies.

## Usage

```cpp
#include "logger.hpp"
#include "sinks/console_sink.hpp"
#include "sinks/file_sink.hpp"

auto& logger = flashlog::Logger::instance();
logger.add_sink<flashlog::ConsoleSink>();
logger.add_sink<flashlog::FileSink>("app.log");
logger.start();

LOG_INFO("tick received");
LOG_ERROR("queue overflow");

logger.stop();
```

## Output

2026-08-03 20:15:16.686425 [INFO ] [0x10b81c13] main.cpp:12 tick received
2026-08-03 20:15:16.686426 [ERROR] [0x10b81c13] main.cpp:15 queue overflow

## How it works

Hot path: `LOG_INFO()` → create record → push to lock-free ring buffer → return.
That's it. ~27ns on the calling thread.

A background thread pops records, formats them, writes to sinks (files, console).
Cross-core latency is ~92ns with a live consumer.

The ring buffer uses a sequence-number based protocol — each slot has an 
atomic sequence that tracks whether it's free, being written, or ready to 
read. Producers claim slots with CAS on a shared tail counter, then publish 
via the slot's sequence. Consumer checks sequences to know when data is safe 
to read.

Disabled log levels (e.g. LOG_TRACE when min level is INFO) compile down to 
nothing — 0.16ns, the compiler eliminates them entirely via `if constexpr`.

## Benchmark

GCC 14, -O3, i7-13800H:

```bash
Ring push (cross-core): 91.8 ns
make_record + push: 26.8 ns
Disabled level: 0.16 ns
Push+pop same thread: 33.4 ns
```

## Build

Header-only — just add `include/flashlog/` to your include path. 
C++20 required for `std::source_location` and `std::format`.

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
./flashlog_test
./bench_flashlog    # needs libbenchmark-dev
```
