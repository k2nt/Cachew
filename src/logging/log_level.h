#pragma once

#include <string_view>
#include <stdexcept>

namespace Logging
{
    // Terminal color codes for log levels
    constexpr std::string_view CYAN = "\033[36m";
    constexpr std::string_view YELLOW = "\033[33m";
    constexpr std::string_view RED = "\033[31m";
    constexpr std::string_view RESET = "\033[0m";

    /**
     * Represents the severity level of a log message.
     */
    enum class LogLevel
    {
        Debug,
        Info,
        Warning,
        Error
    };

    /**
     * Converts a LogLevel enum value to its corresponding string representation.
     * 
     * @param level The LogLevel value to convert.
     * @return A string view representing the log level.
     */
    constexpr std::string_view LogLevelToStringView(LogLevel level)
    {
        switch (level)
        {
        case LogLevel::Debug:
            return "DEBUG";

        case LogLevel::Info:
            return "INFO";
        
        case LogLevel::Warning:
            return "WARNING";

        case LogLevel::Error:
            return "ERROR";

        default:
            throw std::invalid_argument("LogLevel not supported for string conversion");
        }
    }

    /**
     * Returns the terminal color code for the given log level.
     * 
     * @param level The LogLevel value to determine the color.
     * @return A view of a static color code, or an empty view for Info.
     */
    constexpr std::string_view LogLevelTerminalColor(LogLevel level)
    {
        switch (level)
        {
        case LogLevel::Debug:
            return CYAN;

        case LogLevel::Info:
            return "";

        case LogLevel::Warning:
            return YELLOW;

        case LogLevel::Error:
            return RED;

        default:
            throw std::invalid_argument("LogLevel not supported for terminal color");
        }
    }
}
