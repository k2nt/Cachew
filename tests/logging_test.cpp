#include <gtest/gtest.h>

#include <regex>
#include <sstream>
#include <string>

#include "../src/logging/logger.h"

TEST(Logging, FormatsColoredAndPlainLogLines)
{
    const struct
    {
        void (Logging::Logger::*log)(std::string_view);
        std::string_view colorPattern;
        std::string_view level;
    } cases[] = {
        {&Logging::Logger::LogDebug, "\033\\[36m", "DEBUG"},
        {&Logging::Logger::LogInfo, "", "INFO"},
        {&Logging::Logger::LogWarning, "\033\\[33m", "WARNING"},
        {&Logging::Logger::LogError, "\033\\[31m", "ERROR"}
    };

    for (const bool useColor : {true, false})
    {
        for (const auto& testCase : cases)
        {
            SCOPED_TRACE(testCase.level);
            SCOPED_TRACE(useColor);
            std::ostringstream output;
            Logging::Logger logger{output, useColor};

            (logger.*testCase.log)("message");

            std::string pattern = useColor ? std::string(testCase.colorPattern) : "";
            pattern += R"(\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}Z \[)";
            pattern += testCase.level;
            pattern += R"(\] message)";
            if (useColor)
            {
                pattern += "\033\\[0m";
            }
            pattern += '\n';
            EXPECT_TRUE(std::regex_match(output.str(), std::regex(pattern)))
                << output.str();
        }
    }
}

TEST(Logging, PropagatesStreamExceptions)
{
    std::ostringstream output;
    output.setstate(std::ios::badbit);
    EXPECT_THROW(output.exceptions(std::ios::badbit), std::ios_base::failure);
    Logging::Logger logger{output};

    EXPECT_THROW(logger.LogDebug("message"), std::ios_base::failure);
    EXPECT_THROW(logger.LogInfo("message"), std::ios_base::failure);
    EXPECT_THROW(logger.LogWarning("message"), std::ios_base::failure);
    EXPECT_THROW(logger.LogError("message"), std::ios_base::failure);
}
