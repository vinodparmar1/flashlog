
#include <iostream>
#include <thread>
#include "logger.hpp"
#include "sinks/console_sink.hpp"

int main() {
    std::cout << "flashlog test\n";
    auto& logger = flashlog::Logger::instance();
    logger.add_sink<flashlog::ConsoleSink>();
    logger.start();
    LOG_INFO("info msg");
    LOG_TRACE("trace msg");
    LOG_DEBUG("debug msg");
    LOG_ERROR("error msg");

    std::thread t1([]() {
        for(int i =0; i< 10; ++i)
            LOG_INFO("thread1-message");
    });

    std::thread t2([]() {
        for(int i =0; i< 10; ++i)
            LOG_WARN("thread2-message");
    });
    t1.join();
    t2.join();
    logger.stop();
    return 0;
}

/* // What the user writes:
flashlog::Logger logger;
logger.add_sink<FileSink>("/var/log/app.log");
logger.add_sink<ConsoleSink>();
logger.start();

LOG_INFO("tick received");
LOG_ERROR("queue overflow");
*/