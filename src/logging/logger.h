#pragma once

#include <iosfwd>
#include <string_view>

#include "log_level.h"

namespace Logging
{

/**
 * Logger class responsible for logging messages with different severity levels.
 */
class Logger
{
public:
    /// Set isTerminalColor to false for plain output, such as files or pipes.
    explicit Logger(std::ostream& output, bool isTerminalColor = true);

private:
    std::ostream& m_output;
    bool m_isTerminalColor;

public:
    void LogDebug(std::string_view message);

    void LogInfo(std::string_view message);

    void LogWarning(std::string_view message);

    void LogError(std::string_view message);

private:
    void Log(LogLevel level, std::string_view message);
};

};
