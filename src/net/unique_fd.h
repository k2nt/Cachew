#pragma once

namespace Net
{

class UniqueFd
{
private:
    int m_fd;

public:
    explicit UniqueFd(int fd);

    ~UniqueFd();

    // Disable copy semantics, ownership of the file descriptor is unique to this object.
    UniqueFd(const UniqueFd&) = delete;
    UniqueFd& operator=(const UniqueFd&) = delete;

    // Allow move semantics, ownership of the file descriptor can be transferred.
    // T&& refers to a temporary rvalue, allowing the transfer of ownership from one UniqueFd instance to another.
    UniqueFd(UniqueFd&& other) noexcept;
    UniqueFd& operator=(UniqueFd&& other) noexcept;

    [[nodiscard]] int Get() noexcept;
};

}