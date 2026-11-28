#include <cstdlib>
#include <exception>
#include <format>
#include <iostream>

#include "logging/logging.h"
#include "net/tcp_server.h"

int main()
{
    try
    {
        Net::TcpServer server{};
        Logging::Logger logger{std::cerr};

        logger.LogInfo(std::format("Listening on 127.0.0.1:{}", server.GetPort()));

        server.Run();
    }
    catch (const std::exception& error)
    {
        std::cerr << "Fatal error: " << error.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
