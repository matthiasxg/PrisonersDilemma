#pragma once
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#pragma GCC diagnostic pop
#include <string>


class LoggingProvider {
public:
    static LoggingProvider& getInstance() {
        static LoggingProvider logger;
        return logger;
    }

    ~LoggingProvider() {}

    void info(std::string message) {
        spdlog::info(message);
    }

    void warning(std::string message) {
        spdlog::warn(message);
    }
    
    void error(std::string message) {
        spdlog::error(message);
    }
    
    void debug(std::string message) {
        spdlog::debug(message);
    }
};