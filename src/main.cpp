#include <iostream>

#include "logging/logging.h"

int main()
{
    Logging::Logger logger{std::cout};
    logger.LogDebug("Hello world!");
    logger.LogInfo("This is an info message.");
    logger.LogWarning("This is a warning message.");
    logger.LogError("This is an error message.");
}
