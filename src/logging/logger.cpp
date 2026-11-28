#include <format>
#include <chrono>
#include <ostream>

#include "logger.h"
#include "log_level.h"

namespace Logging
{

Logger::Logger(std::ostream& output, bool isTerminalColor)
    : m_output(output), m_isTerminalColor(isTerminalColor) {}

void Logger::LogDebug(std::string_view message)
{
    this->Log(LogLevel::Debug, message);
}

void Logger::LogInfo(std::string_view message)
{
    this->Log(LogLevel::Info, message);
}

void Logger::LogWarning(std::string_view message)
{
    this->Log(LogLevel::Warning, message);
}

void Logger::LogError(std::string_view message)
{
    this->Log(LogLevel::Error, message);
}

void Logger::Log(LogLevel level, std::string_view message)
{
    const auto now = std::chrono::floor<std::chrono::seconds>(
        std::chrono::system_clock::now());

    auto levelName = LogLevelToStringView(level);
    const auto color = m_isTerminalColor ? LogLevelTerminalColor(level) : std::string_view{};
    const auto reset = m_isTerminalColor ? RESET : std::string_view{};

    m_output << std::format(
        "{}{:%FT%TZ} [{}] {}{}\n",
        color,
        now,
        levelName,
        message,
        reset);
}
    
}
