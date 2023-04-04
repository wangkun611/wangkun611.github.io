#define SPDLOG_ACTIVE_LEVEL 0
#include "spdlog/spdlog.h"

#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/async.h"
#include "spdlog/sinks/basic_file_sink.h"
int main() 
{
    spdlog::info("Welcome to spdlog!");
    spdlog::error("Some error message with arg: {}", 1);
    
    spdlog::warn("Easy padding in numbers like {:08d}", 12);
    spdlog::critical("Support for int: {0:d};  hex: {0:x};  oct: {0:o}; bin: {0:b}", 42);
    spdlog::info("Support for floats {:03.2f}", 1.23456);
    spdlog::info("Positional args are {1} {0}..", "too", "supported");
    spdlog::info("{:<30}", "left aligned");
    
    spdlog::set_level(spdlog::level::debug); // Set global log level to debug
    spdlog::debug("This message should be displayed..");    
    
    // change log pattern
    spdlog::set_pattern("[%H:%M:%S %z] [%n] [%^---%L---%$] [thread %t] %v");
    
    // Compile time log levels
    // define SPDLOG_ACTIVE_LEVEL to desired level
    SPDLOG_TRACE("Some trace message with param {}", 42);
    SPDLOG_DEBUG("Some debug message");

    auto logger_mt = spdlog::basic_logger_mt("basic_logger_mt", "logs/basic-log-mt.txt");
    logger_mt->info("File logger mt");

    auto logger_st = spdlog::basic_logger_st("basic_logger_st", "logs/basic-log-st.txt");
    logger_st->info("File logger st");

    auto max_size = 5 * 1024 * 1024;
    auto max_files = 3;
    auto logger_rotating = spdlog::rotating_logger_mt("rotating_logger", "logs/rotating.txt", max_size, max_files);
    logger_rotating->info("Rotating logger");

    // Create a daily logger - a new file is created every day on 2:30am
    int rotation_hour = 2;
    int rotation_minute = 30;
    int max_files_daily = 3;
    auto logger = spdlog::daily_logger_mt("daily_logger", "logs/daily.txt", 2, 30, false, max_files_daily);

    // Create async logger
    auto async_file = spdlog::create_async<spdlog::sinks::basic_file_sink_mt>("async_file_logger", "logs/async_log.txt");
    async_file->info("async_file_logger");

    auto async_file_daily = spdlog::create_async<spdlog::sinks::daily_file_sink_mt>("async_file_logger_daily", "logs/async_log_daily.txt", 2, 30);
    async_file_daily->info("async_file_logger_daily");
}
