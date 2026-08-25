#pragma once

#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>

enum class LogLevel : int
{
    Debug = 0,
    Info,
    Warning,
    Error,
    Fatal
};

class Logger
{
public:
    static Logger &instance();

    inline void setLevel(LogLevel level) { currentLevel_ = level; }

    inline void setFile(const std::string &path, bool append = true);

    inline void log(LogLevel level, const std::string &tag, const std::string &message);

private:
    Logger() = default;
    Logger(const Logger &) = delete;
    Logger &operator=(const Logger &) = delete;

    inline static const char *levelToString(LogLevel level);

    LogLevel currentLevel_ = LogLevel::Info;
    std::mutex mutex_;
    std::ofstream fileStream_;
};

inline Logger &Logger::instance()
{
    static Logger logger;
    return logger;
}

inline void Logger::setFile(const std::string &path, bool append)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (fileStream_.is_open())
        fileStream_.close();

    if (append)
        fileStream_.open(path, std::ios::app);

    else
        fileStream_.open(path, std::ios::trunc);
}

inline const char *Logger::levelToString(LogLevel level)
{
    switch (level)
    {
    case LogLevel::Debug:
        return "DEBUG";

    case LogLevel::Info:
        return "INFO ";

    case LogLevel::Warning:
        return "WARN ";

    case LogLevel::Error:
        return "ERROR";

    case LogLevel::Fatal:
        return "FATAL";
    }

    return "?????";
}

inline void Logger::log(LogLevel level, const std::string &tag, const std::string &message)
{
    if (level < currentLevel_)
        return;

    auto now = std::chrono::system_clock::now();
    auto timeT = std::chrono::system_clock::to_time_t(now);
    std::tm tmBuf{};

#ifdef _WIN32
    localtime_s(&tmBuf, &timeT);
#else
    localtime_r(&timeT, &tmBuf);
#endif

    std::ostringstream line;
    line << std::put_time(&tmBuf, "%Y-%m-%d %H:%M:%S")
         << " [" << levelToString(level) << "] "
         << "[" << tag << "] " << message << "\n";

    std::lock_guard<std::mutex> lock(mutex_);

    if (level >= LogLevel::Error)
        std::cerr << line.str();

    else
        std::cout << line.str();

    if (fileStream_.is_open())
        fileStream_ << line.str() << std::flush;
}

#ifdef NDEBUG
#define LOG_DEBUG(tag, msg) ((void)0)
#else
#define LOG_DEBUG(tag, msg) Logger::instance().log(LogLevel::Debug, tag, msg)
#endif

#define LOG_INFO(tag, msg) Logger::instance().log(LogLevel::Info, tag, msg)
#define LOG_WARNING(tag, msg) Logger::instance().log(LogLevel::Warning, tag, msg)
#define LOG_ERROR(tag, msg) Logger::instance().log(LogLevel::Error, tag, msg)
#define LOG_FATAL(tag, msg) Logger::instance().log(LogLevel::Fatal, tag, msg)
