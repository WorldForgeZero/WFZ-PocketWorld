
#include "logger.h"

#include <chrono>
#include <cstdarg>
#include <cstdio>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <mutex>
#include <string>
#include <system_error>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

namespace fs = std::filesystem;

namespace
{
    constexpr const char *LOG_DIRECTORY = "logs";
    constexpr const char *LOG_FILENAME = "wfz.log";

    std::ofstream g_log_file;
    std::mutex g_log_mutex;

    const char *LogLevelName(wfz::logger::Level level)
    {
        switch (level)
        {
        case wfz::logger::Level::Trace:
            return "TRACE";

        case wfz::logger::Level::Debug:
            return "DEBUG";

        case wfz::logger::Level::Info:
            return "INFO";

        case wfz::logger::Level::Warning:
            return "WARNING";

        case wfz::logger::Level::Error:
            return "ERROR";

        case wfz::logger::Level::Fatal:
            return "FATAL";

        default:
            return "UNKNOWN";
        }
    }

    std::string FormatMessage(const char *format, va_list args)
    {
        va_list copy;
        va_copy(copy, args);

        const int length = std::vsnprintf(
            nullptr, 0, format, copy);

        va_end(copy);

        if (length < 0)
            return "Log formatting failed";

        if (length == 0)
            return {};

        std::string result(static_cast<std::size_t>(length), '\0');

        va_copy(copy, args);

        std::vsnprintf(result.data(), result.size() + 1, format, copy);

        va_end(copy);

        return result;
    }

    std::string BuildPrefix(wfz::logger::Level level)
    {
        const auto now = std::chrono::system_clock::now();
        const std::time_t time = std::chrono::system_clock::to_time_t(now);

        std::tm local_time{};

#ifdef _WIN32
        localtime_s(&local_time, &time);
#else
        localtime_r(&time, &local_time);
#endif

        char timestamp[20]{};

        std::strftime(
            timestamp,
            sizeof(timestamp),
            "%Y-%m-%d %H:%M:%S",
            &local_time);

        std::string prefix;
        prefix.reserve(48);

        prefix += '[';
        prefix += timestamp;
        prefix += "] [";
        prefix += LogLevelName(level);
        prefix += "] ";

        return prefix;
    }

    fs::path GetExecutableDirectory(std::error_code &ec)
    {
#ifdef _WIN32
        std::wstring buffer(512, L'\0');

        for (;;)
        {
            const DWORD length = GetModuleFileNameW(
                nullptr,
                buffer.data(),
                static_cast<DWORD>(buffer.size()));

            if (length == 0)
            {
                ec = std::error_code(
                    static_cast<int>(GetLastError()),
                    std::system_category());

                return {};
            }

            if (length < buffer.size())
            {
                buffer.resize(length);
                ec.clear();

                return fs::path(buffer).parent_path();
            }

            if (buffer.size() >= 32768)
            {
                ec = std::make_error_code(
                    std::errc::filename_too_long);

                return {};
            }

            buffer.resize(buffer.size() * 2);
        }

#elif defined(__linux__)
        const fs::path executable = fs::read_symlink("/proc/self/exe", ec);

        if (ec)
            return {};

        return executable.parent_path();

#else
        ec = std::make_error_code(std::errc::function_not_supported);

        return {};
#endif
    }

    void Write(wfz::logger::Level level, const std::string &message)
    {
        const std::string line = BuildPrefix(level) + message;

        std::lock_guard<std::mutex> lock(g_log_mutex);

        std::ostream &output =
            (level >= wfz::logger::Level::Warning)
                ? std::cerr
                : std::cout;

        output << line << '\n';

        if (g_log_file.is_open())
        {
            g_log_file << line << '\n';
            g_log_file.flush();
        }
    }

    void LogV(wfz::logger::Level level, const char *format, va_list args)
    {
        if (!format)
            return;

        Write(level, FormatMessage(format, args));
    }
}

namespace wfz::logger
{

    bool Init()
    {
        std::lock_guard<std::mutex> lock(g_log_mutex);

        if (g_log_file.is_open())
            return true;

        std::error_code ec;

        const fs::path executable_directory =
            GetExecutableDirectory(ec);

        if (ec)
        {
            std::cerr << "Failed to resolve executable directory: " << ec.message() << '\n';

            return false;
        }

        const fs::path log_directory = executable_directory / LOG_DIRECTORY;

        fs::create_directories(log_directory, ec);

        if (ec)
        {
            std::cerr << "Failed to create log directory: " << ec.message() << '\n';

            return false;
        }

        const fs::path log_path = log_directory / LOG_FILENAME;

        g_log_file.open(log_path, std::ios::out | std::ios::trunc);

        if (!g_log_file.is_open())
        {
            std::cerr << "Failed to open log file: " << log_path << '\n';

            return false;
        }

        return true;
    }

    void Shutdown()
    {
        std::lock_guard<std::mutex> lock(g_log_mutex);

        if (g_log_file.is_open())
        {
            g_log_file.flush();
            g_log_file.close();
        }
    }

    void Log(Level level, const char *format, ...)
    {
        va_list args;
        va_start(args, format);
        LogV(level, format, args);
        va_end(args);
    }

    void Trace(const char *format, ...)
    {
        va_list args;
        va_start(args, format);
        LogV(Level::Trace, format, args);
        va_end(args);
    }

    void Debug(const char *format, ...)
    {
        va_list args;
        va_start(args, format);
        LogV(Level::Debug, format, args);
        va_end(args);
    }

    void Info(const char *format, ...)
    {
        va_list args;
        va_start(args, format);
        LogV(Level::Info, format, args);
        va_end(args);
    }

    void Warning(const char *format, ...)
    {
        va_list args;
        va_start(args, format);
        LogV(Level::Warning, format, args);
        va_end(args);
    }

    void Error(const char *format, ...)
    {
        va_list args;
        va_start(args, format);
        LogV(Level::Error, format, args);
        va_end(args);
    }

    void Fatal(const char *format, ...)
    {
        va_list args;
        va_start(args, format);
        LogV(Level::Fatal, format, args);
        va_end(args);
    }
}
