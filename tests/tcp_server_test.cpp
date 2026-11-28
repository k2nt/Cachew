#include <gtest/gtest.h>

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <string>
#include <string_view>
#include <system_error>
#include <netinet/in.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../src/net/tcp_server.h"

namespace
{

class ServerProcess
{
public:
    explicit ServerProcess(Net::TcpServer& server)
        : m_pid(::fork())
    {
        if (m_pid == -1)
        {
            throw std::system_error(errno, std::generic_category(), "fork failed");
        }
        if (m_pid == 0)
        {
            ::alarm(15);
            try
            {
                server.Run();
            }
            catch (const std::exception& error)
            {
                std::fprintf(stderr, "Test server failed: %s\n", error.what());
            }
            ::_exit(EXIT_FAILURE);
        }
    }

    ~ServerProcess()
    {
        EXPECT_EQ(::kill(m_pid, SIGTERM), 0);
        int status = 0;
        pid_t result;
        do
        {
            result = ::waitpid(m_pid, &status, 0);
        } while (result == -1 && errno == EINTR);
        EXPECT_EQ(result, m_pid);
        EXPECT_TRUE(WIFSIGNALED(status));
        if (WIFSIGNALED(status))
        {
            EXPECT_EQ(WTERMSIG(status), SIGTERM);
        }
    }

    ServerProcess(const ServerProcess&) = delete;
    ServerProcess& operator=(const ServerProcess&) = delete;

private:
    pid_t m_pid;
};

Net::UniqueFd Connect(std::uint16_t port)
{
    Net::UniqueFd client{::socket(AF_INET, SOCK_STREAM, 0)};
    if (!client.Valid())
    {
        throw std::system_error(errno, std::generic_category(), "socket failed");
    }

    const timeval timeout{2, 0};
    if (::setsockopt(
        client.Get(),
        SOL_SOCKET,
        SO_RCVTIMEO,
        &timeout,
        sizeof(timeout)) == -1)
    {
        throw std::system_error(
            errno,
            std::generic_category(),
            "Failed to set test receive timeout");
    }
    if (::setsockopt(
        client.Get(),
        SOL_SOCKET,
        SO_SNDTIMEO,
        &timeout,
        sizeof(timeout)) == -1)
    {
        throw std::system_error(
            errno,
            std::generic_category(),
            "Failed to set test send timeout");
    }
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
            "Failed to set test SO_NOSIGPIPE");
    }
#endif

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    if (::connect(
        client.Get(),
        reinterpret_cast<const sockaddr*>(&address),
        sizeof(address)) == -1)
    {
        throw std::system_error(errno, std::generic_category(), "connect failed");
    }
    return client;
}

void ExpectEcho(const Net::UniqueFd& client, std::string_view input)
{
#ifdef MSG_NOSIGNAL
    constexpr int sendFlags = MSG_NOSIGNAL;
#else
    constexpr int sendFlags = 0;
#endif
    std::size_t sent = 0;
    while (sent < input.size())
    {
        const auto count = ::send(
            client.Get(),
            input.data() + sent,
            input.size() - sent,
            sendFlags);
        ASSERT_GT(count, 0) << std::strerror(errno);
        sent += static_cast<std::size_t>(count);
    }

    std::string response(input.size(), '\0');
    std::size_t received = 0;
    while (received < response.size())
    {
        const auto count = ::recv(
            client.Get(),
            response.data() + received,
            response.size() - received,
            0);
        ASSERT_GT(count, 0) << std::strerror(errno);
        received += static_cast<std::size_t>(count);
    }
    EXPECT_EQ(response, input);
}

}

TEST(TcpServer, ReportsAssignedPortAndReleasesListener)
{
    std::uint16_t port;
    {
        Net::TcpServer server{0};
        port = server.GetPort();
        ASSERT_NE(port, 0);

        try
        {
            Net::TcpServer duplicate{port};
            FAIL() << "Binding to an occupied port unexpectedly succeeded";
        }
        catch (const std::system_error& error)
        {
            EXPECT_EQ(error.code().value(), EADDRINUSE);
        }
    }
    EXPECT_NO_THROW(Net::TcpServer replacement{port});
}

TEST(TcpServer, EchoesTextAndBinaryDataAndAcceptsSubsequentClients)
{
    Net::TcpServer server{0};
    ServerProcess process{server};

    for (int connection = 0; connection < 2; ++connection)
    {
        auto client = Connect(server.GetPort());
        ExpectEcho(client, "hello\r\n");
        ExpectEcho(client, std::string_view{"a\0b", 3});
    }
}

TEST(TcpServer, EchoesInputLargerThanReadBuffer)
{
    Net::TcpServer server{0};
    ServerProcess process{server};
    auto client = Connect(server.GetPort());
    std::string input(10000, 'x');
    input[0] = '\0';
    input[4095] = '\r';
    input[4096] = '\n';
    input.back() = 'z';

    ExpectEcho(client, input);
}

TEST(TcpServer, ContinuesAfterClientResetsConnection)
{
    Net::TcpServer server{0};
    ServerProcess process{server};
    {
        auto client = Connect(server.GetPort());
        ExpectEcho(client, "before reset");
        const linger reset{1, 0};
        ASSERT_EQ(
            ::setsockopt(
                client.Get(),
                SOL_SOCKET,
                SO_LINGER,
                &reset,
                sizeof(reset)),
            0);
#ifdef MSG_NOSIGNAL
        constexpr int sendFlags = MSG_NOSIGNAL;
#else
        constexpr int sendFlags = 0;
#endif
        ASSERT_EQ(::send(client.Get(), "x", 1, sendFlags), 1);
    }

    auto nextClient = Connect(server.GetPort());
    ExpectEcho(nextClient, "after reset");
}
