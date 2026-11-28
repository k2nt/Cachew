#include <array>
#include <cerrno>
#include <chrono>
#include <iostream>
#include <string_view>
#include <system_error>
#include <thread>
#include <netinet/in.h>
#include <sys/socket.h>

#include "tcp_server.h"
#include "../logging/logger.h"

namespace Net
{

TcpServer::TcpServer(std::uint16_t port, std::uint16_t backlog)
    : m_listener(::socket(AF_INET, SOCK_STREAM, 0)),
      m_port(port),
      m_backlog(backlog)
{
    if (!this->m_listener.Valid())
    {
        throw std::system_error(
            errno, 
            std::generic_category(),
            "Failed to create socket");
    }

    int enabled = 1;

    const int optResult = ::setsockopt(
        m_listener.Get(),
        SOL_SOCKET,
        SO_REUSEADDR,
        &enabled,
        sizeof(enabled));
    
    if (optResult == -1)
    {
        throw std::system_error(
            errno,
            std::generic_category(),
            "Failed to set SO_REUSEADDR");
    }

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_port = htons(this->m_port);
    address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    const int bindResult = ::bind(
        m_listener.Get(),
        reinterpret_cast<const sockaddr*>(&address),
        sizeof(address));

    if (bindResult == -1)
    {
        throw std::system_error(
            errno,
            std::generic_category(),
            "Failed to bind socket");
    }

    if (::listen(m_listener.Get(), m_backlog) == -1)
    {
        throw std::system_error(
            errno,
            std::generic_category(),
            "Failed to listen on socket");
    }

    socklen_t addressLength = sizeof(address);
    if (::getsockname(
        m_listener.Get(),
        reinterpret_cast<sockaddr*>(&address),
        &addressLength) == -1)
    {
        throw std::system_error(
            errno,
            std::generic_category(),
            "Failed to obtain listening port");
    }
    m_port = ntohs(address.sin_port);
}

std::uint16_t TcpServer::GetPort() const noexcept
{
    return m_port;
}

void TcpServer::Run()
{
    Logging::Logger logger{std::cerr, false};

    for (;;)
    {
        UniqueFd client{::accept(m_listener.Get(), nullptr, nullptr)};
        if (!client.Valid())
        {
            if (errno == EINTR)
            {
                continue;
            }

            logger.LogError(std::system_error(
                errno,
                std::generic_category(),
                "Failed to accept client").what());
            // Avoid a busy retry loop when descriptors or socket buffers run out.
            std::this_thread::sleep_for(std::chrono::milliseconds{100});
            continue;
        }

        try
        {
            HandleClient(client);
        }
        catch (const std::system_error& error)
        {
            logger.LogWarning(error.what());
        }
    }
}

void TcpServer::HandleClient(const UniqueFd& client)
{
#ifdef SO_NOSIGPIPE
    const int enabled = 1;
    if (::setsockopt(
        client.Get(),
        SOL_SOCKET,
        SO_NOSIGPIPE,
        &enabled,
        sizeof(enabled)) == -1)
    {
        throw std::system_error(
            errno,
            std::generic_category(),
            "Failed to set SO_NOSIGPIPE");
    }
#endif

#ifdef MSG_NOSIGNAL
    constexpr int sendFlags = MSG_NOSIGNAL;
#else
    constexpr int sendFlags = 0;
#endif

    std::array<char, 4096> buffer;

    for (;;)
    {
        const auto received = ::recv(client.Get(), buffer.data(), buffer.size(), 0);
        if (received == 0)
        {
            return;
        }
        if (received == -1)
        {
            if (errno == EINTR)
            {
                continue;
            }
            throw std::system_error(
                errno,
                std::generic_category(),
                "Failed to receive from client");
        }

        const std::string_view response{buffer.data(), static_cast<std::size_t>(received)};
        std::size_t sent = 0;
        while (sent < response.size())
        {
            const auto written = ::send(
                client.Get(),
                response.data() + sent,
                response.size() - sent,
                sendFlags);
            if (written == -1)
            {
                if (errno == EINTR)
                {
                    continue;
                }
                throw std::system_error(
                    errno,
                    std::generic_category(),
                    "Failed to send to client");
            }
            if (written == 0)
            {
                throw std::system_error(
                    std::make_error_code(std::errc::io_error),
                    "Sending to client made no progress");
            }
            sent += static_cast<std::size_t>(written);
        }
    }
}

}
