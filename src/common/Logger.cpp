#include "MotArda/common/Logger.hpp"
#include <iostream>

namespace MTRD {

    std::shared_ptr<spdlog::logger> Logger::s_logger = nullptr;

    Logger& Logger::getInstance() {
        static Logger instance;
        return instance;
    }

    void Logger::init(const char* loggerName, Level level) {
        if (s_logger) return;

        auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        consoleSink->set_pattern("[%T] [%^%l%$] [%n]: %v");

        try {
            s_logger = std::make_shared<spdlog::logger>(loggerName, consoleSink);
            s_logger->set_level(spdlog::level::trace);
            s_logger->flush_on(spdlog::level::warn);
            setLevel(level);
            s_logger->info("Logger initialized successfully");
        } catch (const spdlog::spdlog_ex& ex) {
            std::cerr << "Logger initialization failed: " << ex.what() << std::endl;
        }
    }

    void Logger::shutdown() {
        if (s_logger) {
            s_logger->info("Logger shutting down");
            spdlog::shutdown();
            s_logger.reset();
        }
    }

    void Logger::setLevel(Level level) {
        if (!s_logger) return;

        spdlog::level::level_enum spdLevel;
        switch (level) {
            case Level::Trace:   spdLevel = spdlog::level::trace; break;
            case Level::Debug:   spdLevel = spdlog::level::debug; break;
            case Level::Info:    spdLevel = spdlog::level::info; break;
            case Level::Warn:    spdLevel = spdlog::level::warn; break;
            case Level::Error:   spdLevel = spdlog::level::err; break;
            case Level::Critical: spdLevel = spdlog::level::critical; break;
            default:             spdLevel = spdlog::level::info; break;
        }
        s_logger->set_level(spdLevel);
    }

    void Logger::trace(const char* message) {
        if (s_logger) s_logger->trace(message);
    }

    void Logger::debug(const char* message) {
        if (s_logger) s_logger->debug(message);
    }

    void Logger::info(const char* message) {
        if (s_logger) s_logger->info(message);
    }

    void Logger::warn(const char* message) {
        if (s_logger) s_logger->warn(message);
    }

    void Logger::error(const char* message) {
        if (s_logger) s_logger->error(message);
    }

    void Logger::critical(const char* message) {
        if (s_logger) s_logger->critical(message);
    }

}
