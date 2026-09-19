// ---------------------
// External definitions.
// ---------------------

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using ::testing::AllOf;
using ::testing::HasSubstr;

using ::testing::_;
using ::testing::InSequence;
using ::testing::SetErrnoAndReturn;

#include <arpa/inet.h>

// ---------------------
// Internal definitions.
// ---------------------

#include <errcodes.h>

#include <tcprtns.h>

TEST(TcpSetSockAddr, IsOk)
{
    // Arrange
    struct sockaddr_in sockaddr;
    int sockaddr_len = sizeof(sockaddr);

    // Act
    TcpSetSockAddr("localhost", "ftp", &sockaddr, sockaddr_len);

    // Assert
    EXPECT_EQ(21, ntohs(sockaddr.sin_port));

}

TEST(TcpSetSockAddrDeathTest, DiesInvalidService)
{
    EXPECT_EXIT(
    {
        // Arrange
        struct sockaddr_in sockaddr;
        int sockaddr_len = sizeof(sockaddr);

        // Act
        TcpSetSockAddr("localhost", "SomeInvalidService", &sockaddr, sockaddr_len);
    },
    // Assert
    ::testing::ExitedWithCode(1),
    HasSubstr("code: " + std::to_string(SYSCALL_GETADDRINFO)));
}