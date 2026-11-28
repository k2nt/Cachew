#pragma once

#include <cstdint>

#include "unique_fd.h"

namespace Net
{

constexpr std::uint16_t DEFAULT_PORT = 5123;
constexpr std::uint16_t DEFAULT_BACKLOG = 16;

/**
 * TCP server class that listens on a specified port and handles incoming connections.
 */
class TcpServer
{
private:
    UniqueFd m_listener;
    std::uint16_t m_port;
    std::uint16_t m_backlog;

public:
    explicit TcpServer(
        std::uint16_t port = DEFAULT_PORT,
        std::uint16_t backlog = DEFAULT_BACKLOG);

    [[nodiscard]] std::uint16_t GetPort() const noexcept;

    // Blocks and serves one client at a time, echoing received bytes unchanged.
    // RESP framing and graceful shutdown are not implemented at this milestone.
    void Run();

private:
    void HandleClient(const UniqueFd& client);
};

}
