#pragma once

#include <memory>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <fmt/core.h>

namespace MTRD {

    class Logger {
    public:
        enum class Level {
            Trace,
            Debug,
            Info,
            Warn,
            Error,
            Critical
        };

        static void init(const char* loggerName = "MotArda", Level level = Level::Info);
        static void shutdown();

        static void trace(const char* message);
        static void debug(const char* message);
        static void info(const char* message);
        static void warn(const char* message);
        static void error(const char* message);
        static void critical(const char* message);

        static void setLevel(Level level);

        template<typename... Args>
        static void trace(fmt::format_string<Args...> fmt, Args&&... args) {
            if (s_logger) s_logger->trace(fmt, std::forward<Args>(args)...);
        }

        template<typename... Args>
        static void debug(fmt::format_string<Args...> fmt, Args&&... args) {
            if (s_logger) s_logger->debug(fmt, std::forward<Args>(args)...);
        }

        template<typename... Args>
        static void info(fmt::format_string<Args...> fmt, Args&&... args) {
            if (s_logger) s_logger->info(fmt, std::forward<Args>(args)...);
        }

        template<typename... Args>
        static void warn(fmt::format_string<Args...> fmt, Args&&... args) {
            if (s_logger) s_logger->warn(fmt, std::forward<Args>(args)...);
        }

        template<typename... Args>
        static void error(fmt::format_string<Args...> fmt, Args&&... args) {
            if (s_logger) s_logger->error(fmt, std::forward<Args>(args)...);
        }

        template<typename... Args>
        static void critical(fmt::format_string<Args...> fmt, Args&&... args) {
            if (s_logger) s_logger->critical(fmt, std::forward<Args>(args)...);
        }

    private:
        Logger() = default;
        ~Logger() = default;

        Logger(const Logger&) = delete;
        Logger& operator=(const Logger&) = delete;
        Logger(Logger&&) = delete;
        Logger& operator=(Logger&&) = delete;

        static Logger& getInstance();

        static std::shared_ptr<spdlog::logger> s_logger;
    };

}
