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
    UniqueFd(UniqueFd&& other) noexcept;
    UniqueFd& operator=(UniqueFd&& other) noexcept;

    [[nodiscard]] int Get() const noexcept;

    [[nodiscard]] bool Valid() const noexcept;
};

}