#include <unistd.h>
#include <utility>

#include "unique_fd.h"

namespace Net
{

explicit UniqueFd::UniqueFd(int fd)
    : m_fd(fd) {}

UniqueFd::~UniqueFd()
{
    if (m_fd >= 0)
    {
        ::close(m_fd);
    }
}

UniqueFd::UniqueFd(UniqueFd&& other) noexcept
    : m_fd(std::exchange(other.m_fd, -1)) {}

UniqueFd& UniqueFd::operator=(UniqueFd&& other) noexcept
{
    if (this != &other)
    {
        // Close the current file descriptor if it is valid before taking ownership of the new one.
        if (m_fd >= 0)
        {
            ::close(m_fd);
        }

        // Take ownership of the new file descriptor from the other UniqueFd instance.
        m_fd = std::exchange(other.m_fd, -1);
    }
    
    return *this;
}

[[nodiscard]] int UniqueFd::Get() noexcept
{
    return m_fd;
}

}