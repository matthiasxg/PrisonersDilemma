/*
 *  Name: Matthias Grill
 *  Class: 5BHIF
 *  Date: 27.02.2019
 *  File: include/logging_provider.hpp
 */
#pragma once

#define SPDLOG_EOL ""

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
        spdlog::set_pattern("[Prisoners Dilemma] [%^%l%$] %v");
        return logger;
    }

    ~LoggingProvider() {}

    void info(std::string message) {
        spdlog::info(message + "\n");
    }

    void warning(std::string message) {
        spdlog::warn(message + "\n");
    }
    
    void error(std::string message) {
        spdlog::error(message + "\n");
    }
    
    void debug(std::string message) {
        spdlog::debug(message + "\n");
    }

    void input(std::string message) {
        spdlog::set_pattern("[Prisoners Dilemma] [%^Input%$] %v");
        spdlog::info(message);
        spdlog::set_pattern("[Prisoners Dilemma] [%^%l%$] %v");
    }

    void otherPlayer(std::string message) {
        spdlog::set_pattern("[Prisoners Dilemma] [%^Opponent%$] %v");
        warning(message);
        spdlog::set_pattern("[Prisoners Dilemma] [%^%l%$] %v");
    }
};