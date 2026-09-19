// ---------------------
// External definitions.
// ---------------------

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using ::testing::Mock;
using ::testing::AllOf;
using ::testing::HasSubstr;

using ::testing::_;
using ::testing::DoAll;
using ::testing::InSequence;
using ::testing::Mock;
using ::testing::Return;
using ::testing::SetErrnoAndReturn;
using ::testing::SetArgPointee;

#include <errno.h>

#include <sys/stat.h>
#include <arpa/inet.h>
#include <poll.h>

// ---------------------
// Internal definitions.
// ---------------------

#include <mockedfns.hpp>

#include <errcodes.h>
#include <syssizes.h>
#include <typedefs.h>

#include <llsocket.h>
#include <logrtns.h>
#include <wraptcprtns.h>

TEST(TcpSetListenSocketDeathTest, Diessocket)
{
    // We are using mocks in a death test. Since the death test is run in a child process, the expectations
    // also need to be in the child process.
    EXPECT_EXIT(
        {
            // Arrange
            MockedFnsObj = std::make_shared<MockedFns>();

            int listenFd;
            struct sockaddr_in sockaddr;
            int sockaddr_len = sizeof(sockaddr);

            InSequence seq;
 
            int sysErrno = EACCES;

            EXPECT_CALL(*MockedFnsObj, llsocket)
            .Times(1)
            .WillOnce(SetErrnoAndReturn(sysErrno, -1));

            // Act
            TcpSetListenSocket("localhost", "ftp", 0, &listenFd, &sockaddr, sockaddr_len);
        },
        // Assert
        ::testing::ExitedWithCode(1),
        AllOf(HasSubstr("code: " + std::to_string(SYSCALL_SOCKET)),
              HasSubstr("errno: " + std::to_string(EACCES) + " [" + strerror(EACCES) + "]")));
 
}

TEST(TcpSetListenSocketDeathTest, Diesbind)
{
    // We are using mocks in a death test. Since the death test is run in a child process, the expectations
    // also need to be in the child process.
    EXPECT_EXIT(
        {
            // Arrange
            MockedFnsObj = std::make_shared<MockedFns>();

            int listenFd;
            struct sockaddr_in sockaddr;
            int sockaddr_len = sizeof(sockaddr);

            InSequence seq;
 
            int sysErrno = EACCES;

            EXPECT_CALL(*MockedFnsObj, llsocket)
            .Times(1)
            .WillOnce(Return(3));

            EXPECT_CALL(*MockedFnsObj, llbind)
            .Times(1)
            .WillOnce(SetErrnoAndReturn(sysErrno, -1));

            // Act
            TcpSetListenSocket("localhost", "ftp", 0, &listenFd, &sockaddr, sockaddr_len);
        },
        // Assert
        ::testing::ExitedWithCode(1),
        AllOf(HasSubstr("code: " + std::to_string(SYSCALL_BIND)),
              HasSubstr("errno: " + std::to_string(EACCES) + " [" + strerror(EACCES) + "]")));
 
}

TEST(TcpSetListenSocketDeathTest, Dieslisten)
{
    // We are using mocks in a death test. Since the death test is run in a child process, the expectations
    // also need to be in the child process.
    EXPECT_EXIT(
        {
            // Arrange
            MockedFnsObj = std::make_shared<MockedFns>();

            int listenFd;
            struct sockaddr_in sockaddr;
            int sockaddr_len = sizeof(sockaddr);

            InSequence seq;
 
            int sysErrno = EADDRINUSE;

            EXPECT_CALL(*MockedFnsObj, llsocket)
            .Times(1)
            .WillOnce(Return(3));

            EXPECT_CALL(*MockedFnsObj, llbind)
            .Times(1)
            .WillOnce(Return(0));

            EXPECT_CALL(*MockedFnsObj, lllisten)
            .Times(1)
            .WillOnce(SetErrnoAndReturn(sysErrno, -1));

            // Act
            TcpSetListenSocket("localhost", "ftp", 0, &listenFd, &sockaddr, sockaddr_len);
        },
        // Assert
        ::testing::ExitedWithCode(1),
        AllOf(HasSubstr("code: " + std::to_string(SYSCALL_LISTEN)),
              HasSubstr("errno: " + std::to_string(EADDRINUSE) + " [" + strerror(EADDRINUSE) + "]")));
}

TEST(TcpSetListenSocket, IsOk)
{
    // Arrange
    MockedFnsObj = std::make_shared<MockedFns>();

    int listenFd;
    struct sockaddr_in sockaddr;
    int sockaddr_len = sizeof(sockaddr);

    InSequence seq;

    EXPECT_CALL(*MockedFnsObj, llsocket)
    .Times(1)
    .WillOnce(Return(3));

    EXPECT_CALL(*MockedFnsObj, llbind)
    .Times(1)
    .WillOnce(Return(0));

    EXPECT_CALL(*MockedFnsObj, lllisten)
    .Times(1)
    .WillOnce(Return(0));

    // Act
    TcpSetListenSocket("localhost", "ftp", 0, &listenFd, &sockaddr, sockaddr_len);

    // Assert
    EXPECT_EQ(3, listenFd);

    bool verified = Mock::VerifyAndClearExpectations(MockedFnsObj.get());
    EXPECT_TRUE(verified);
}

TEST(TcpSetConnectSocketDeathTest, Diessocket)
{
    // We are using mocks in a death test. Since the death test is run in a child process, the expectations
    // also need to be in the child process.
    EXPECT_EXIT(
        {
            // Arrange
            MockedFnsObj = std::make_shared<MockedFns>();

            int socketFd;

            InSequence seq;
 
            int sysErrno = EACCES;

            EXPECT_CALL(*MockedFnsObj, llsocket)
            .Times(1)
            .WillOnce(SetErrnoAndReturn(sysErrno, -1));

            // Act
            TcpSetConnectSocket("localhost", "ftp", &socketFd);
        },
        // Assert
        ::testing::ExitedWithCode(1),
        AllOf(HasSubstr("code: " + std::to_string(SYSCALL_SOCKET)),
              HasSubstr("errno: " + std::to_string(EACCES) + " [" + strerror(EACCES) + "]")));
 
}

TEST(TcpSetConnectSocketDeathTest, Diesconnect)
{
    // We are using mocks in a death test. Since the death test is run in a child process, the expectations
    // also need to be in the child process.
    EXPECT_EXIT(
        {
            // Arrange
            MockedFnsObj = std::make_shared<MockedFns>();

            int socketFd;

            InSequence seq;
 
            int sysErrno = EACCES;

            EXPECT_CALL(*MockedFnsObj, llsocket)
            .Times(1)
            .WillOnce(Return(6));

            EXPECT_CALL(*MockedFnsObj, llconnect)
            .Times(1)
            .WillOnce(SetErrnoAndReturn(sysErrno, -1));

            EXPECT_CALL(*MockedFnsObj, llclose)
            .Times(1)
            .WillOnce(Return(0));

            // Act
            TcpSetConnectSocket("localhost", "ftp", &socketFd);
        },
        // Assert
        ::testing::ExitedWithCode(1),
        AllOf(HasSubstr("code: " + std::to_string(SYSCALL_CONNECT)),
              HasSubstr("errno: " + std::to_string(EACCES) + " [" + strerror(EACCES) + "]")));
 
}

TEST(TcpSetConnectSocket, IsOkValidConnect)
{
    // Arrange
    MockedFnsObj = std::make_shared<MockedFns>();

    int socketFd;

    InSequence seq;

    EXPECT_CALL(*MockedFnsObj, llsocket)
    .Times(1)
    .WillOnce(Return(6));

    EXPECT_CALL(*MockedFnsObj, llconnect)
    .Times(1)
    .WillOnce(Return(0));

    // Act
    int ret = TcpSetConnectSocket("localhost", "ftp", &socketFd);

    // Assert
    EXPECT_EQ(0, ret);
    EXPECT_EQ(6, socketFd);

    bool verified = Mock::VerifyAndClearExpectations(MockedFnsObj.get());
    EXPECT_TRUE(verified);
}

TEST(TcpSetConnectSocket, IsOkNoConnect)
{
    // Arrange
    MockedFnsObj = std::make_shared<MockedFns>();

    int socketFd;

    InSequence seq;

    EXPECT_CALL(*MockedFnsObj, llsocket)
    .Times(1)
    .WillOnce(Return(6));

    EXPECT_CALL(*MockedFnsObj, llconnect)
    .Times(1)
    .WillOnce(SetErrnoAndReturn(ECONNREFUSED, -1));

    EXPECT_CALL(*MockedFnsObj, llclose)
    .Times(1)
    .WillOnce(Return(0));

    // Act
    int ret = TcpSetConnectSocket("localhost", "ftp", &socketFd);

    // Assert
    EXPECT_EQ(1, ret);
    EXPECT_EQ(-1, socketFd);

    bool verified = Mock::VerifyAndClearExpectations(MockedFnsObj.get());
    EXPECT_TRUE(verified);
}

TEST(TcpSetConnectSocketNoBlock, IsOkValidConnect)
{
    // Arrange
    MockedFnsObj = std::make_shared<MockedFns>();

    int socketFd;

    InSequence seq;

    EXPECT_CALL(*MockedFnsObj, llsocket)
    .Times(1)
    .WillOnce(Return(6));

    EXPECT_CALL(*MockedFnsObj, llconnect)
    .Times(1)
    .WillOnce(Return(0));

    EXPECT_CALL(*MockedFnsObj, llfcntl)
    .Times(2)
    .WillRepeatedly(Return(0));

    // Act
    int ret = TcpSetConnectSocketNoBlock("localhost", "ftp", &socketFd);

    // Assert
    EXPECT_EQ(0, ret);
    EXPECT_EQ(6, socketFd);

    bool verified = Mock::VerifyAndClearExpectations(MockedFnsObj.get());
    EXPECT_TRUE(verified);
}

TEST(TcpSetConnectSocketNoBlock, IsOkAfterPoll)
{
    // Arrange
    MockedFnsObj = std::make_shared<MockedFns>();

    int socketFd;
    struct pollfd PollFd;
    PollFd.revents = POLLOUT;

    InSequence seq;

    EXPECT_CALL(*MockedFnsObj, llsocket)
    .Times(1)
    .WillOnce(Return(6));

    EXPECT_CALL(*MockedFnsObj, llconnect)
    .Times(1)
    .WillOnce(SetErrnoAndReturn(EINPROGRESS, -1));

    EXPECT_CALL(*MockedFnsObj, llpoll)
    .Times(1)
    .WillOnce(DoAll(SetArgPointee<0>(PollFd), Return(1)));

    EXPECT_CALL(*MockedFnsObj, llgetsockoptSocketError)
    .Times(1)
    .WillOnce(DoAll(SetArgPointee<1>(0), Return(0)));

    EXPECT_CALL(*MockedFnsObj, llfcntl)
    .Times(2)
    .WillRepeatedly(Return(0));

    // Act
    int ret = TcpSetConnectSocketNoBlock("localhost", "ftp", &socketFd);

    // Assert
    EXPECT_EQ(0, ret);
    EXPECT_EQ(6, socketFd);

    bool verified = Mock::VerifyAndClearExpectations(MockedFnsObj.get());
    EXPECT_TRUE(verified);
}

TEST(TcpSetConnectSocketNoBlock, IsOkAfterPollTimeout)
{
    // Arrange
    MockedFnsObj = std::make_shared<MockedFns>();

    int socketFd;

    InSequence seq;

    EXPECT_CALL(*MockedFnsObj, llsocket)
    .Times(1)
    .WillOnce(Return(6));

    EXPECT_CALL(*MockedFnsObj, llconnect)
    .Times(1)
    .WillOnce(SetErrnoAndReturn(EINPROGRESS, -1));

    EXPECT_CALL(*MockedFnsObj, llpoll)
    .Times(1)
    .WillOnce(Return(0));

    EXPECT_CALL(*MockedFnsObj, llclose)
    .Times(1);

    // Act
    int ret = TcpSetConnectSocketNoBlock("localhost", "ftp", &socketFd);

    // Assert
    EXPECT_EQ(1, ret);
    EXPECT_EQ(-1, socketFd);

    bool verified = Mock::VerifyAndClearExpectations(MockedFnsObj.get());
    EXPECT_TRUE(verified);
}

TEST(TcpSetConnectSocketNoBlock, IsOkAfterPollHangup)
{
    // Arrange
    MockedFnsObj = std::make_shared<MockedFns>();

    int socketFd;
    struct pollfd PollFd;
    PollFd.revents = POLLHUP;

    InSequence seq;

    EXPECT_CALL(*MockedFnsObj, llsocket)
    .Times(1)
    .WillOnce(Return(6));

    EXPECT_CALL(*MockedFnsObj, llconnect)
    .Times(1)
    .WillOnce(SetErrnoAndReturn(EINPROGRESS, -1));

    EXPECT_CALL(*MockedFnsObj, llpoll)
    .Times(1)
    .WillOnce(DoAll(SetArgPointee<0>(PollFd), Return(1)));

    EXPECT_CALL(*MockedFnsObj, llclose)
    .Times(1);

    // Act
    int ret = TcpSetConnectSocketNoBlock("localhost", "ftp", &socketFd);

    // Assert
    EXPECT_EQ(1, ret);
    EXPECT_EQ(-1, socketFd);

    bool verified = Mock::VerifyAndClearExpectations(MockedFnsObj.get());
    EXPECT_TRUE(verified);
}

TEST(TcpSetConnectSocketNoBlock, IsOkAftergetsockoptError)
{
    // Arrange
    MockedFnsObj = std::make_shared<MockedFns>();

    int socketFd;
    struct pollfd PollFd;
    PollFd.revents = POLLOUT;

    InSequence seq;

    EXPECT_CALL(*MockedFnsObj, llsocket)
    .Times(1)
    .WillOnce(Return(6));

    EXPECT_CALL(*MockedFnsObj, llconnect)
    .Times(1)
    .WillOnce(SetErrnoAndReturn(EINPROGRESS, -1));

    EXPECT_CALL(*MockedFnsObj, llpoll)
    .Times(1)
    .WillOnce(DoAll(SetArgPointee<0>(PollFd), Return(1)));

    EXPECT_CALL(*MockedFnsObj, llgetsockoptSocketError)
    .Times(1)
    .WillOnce(DoAll(SetArgPointee<1>(1), Return(0)));

    EXPECT_CALL(*MockedFnsObj, llclose)
    .Times(1);

    // Act
    int ret = TcpSetConnectSocketNoBlock("localhost", "ftp", &socketFd);

    // Assert
    EXPECT_EQ(1, ret);
    EXPECT_EQ(-1, socketFd);

    bool verified = Mock::VerifyAndClearExpectations(MockedFnsObj.get());
    EXPECT_TRUE(verified);
}

TEST(TcpSetConnectSocketNoBlockDeathTest, Diessocket)
{
    // We are using mocks in a death test. Since the death test is run in a child process, the expectations
    // also need to be in the child process.
    EXPECT_EXIT(
        {
            // Arrange
            MockedFnsObj = std::make_shared<MockedFns>();

            int socketFd;

            InSequence seq;
 
            int sysErrno = EACCES;

            EXPECT_CALL(*MockedFnsObj, llsocket)
            .Times(1)
            .WillOnce(SetErrnoAndReturn(sysErrno, -1));

            // Act
            TcpSetConnectSocketNoBlock("localhost", "ftp", &socketFd);
        },
        // Assert
        ::testing::ExitedWithCode(1),
        AllOf(HasSubstr("code: " + std::to_string(SYSCALL_SOCKET)),
              HasSubstr("errno: " + std::to_string(EACCES) + " [" + strerror(EACCES) + "]")));
 
}

TEST(TcpSetConnectSocketNoBlockDeathTest, Diesconnect)
{
    // We are using mocks in a death test. Since the death test is run in a child process, the expectations
    // also need to be in the child process.
    EXPECT_EXIT(
        {
            // Arrange
            MockedFnsObj = std::make_shared<MockedFns>();

            int socketFd = 6;

            InSequence seq;
 
            EXPECT_CALL(*MockedFnsObj, llsocket)
            .Times(1)
            .WillOnce(Return(6));

            int sysErrno = EACCES;

            EXPECT_CALL(*MockedFnsObj, llconnect)
            .Times(1)
            .WillOnce(SetErrnoAndReturn(sysErrno, -1));

            // Act
            TcpSetConnectSocketNoBlock("localhost", "ftp", &socketFd);
        },
        // Assert
        ::testing::ExitedWithCode(1),
        AllOf(HasSubstr("code: " + std::to_string(SYSCALL_CONNECT)),
              HasSubstr("errno: " + std::to_string(EACCES) + " [" + strerror(EACCES) + "]")));
 
}

TEST(TcpSetConnectSocketNoBlockDeathTest, DiesPoll)
{
    // We are using mocks in a death test. Since the death test is run in a child process, the expectations
    // also need to be in the child process.
    EXPECT_EXIT(
        {
            // Arrange
            MockedFnsObj = std::make_shared<MockedFns>();

            int socketFd = 6;

            InSequence seq;
 
            EXPECT_CALL(*MockedFnsObj, llsocket)
            .Times(1)
            .WillOnce(Return(6));

            EXPECT_CALL(*MockedFnsObj, llconnect)
            .Times(1)
            .WillOnce(SetErrnoAndReturn(EINPROGRESS, -1));

            int sysErrno = EACCES;

            EXPECT_CALL(*MockedFnsObj, llpoll)
            .Times(1)
            .WillOnce(SetErrnoAndReturn(sysErrno, -1));

            // Act
            TcpSetConnectSocketNoBlock("localhost", "ftp", &socketFd);
        },
        // Assert
        ::testing::ExitedWithCode(1),
        AllOf(HasSubstr("code: " + std::to_string(SYSCALL_POLL)),
              HasSubstr("errno: " + std::to_string(EACCES) + " [" + strerror(EACCES) + "]")));
 
}

TEST(TcpSetConnectSocketNoBlockDeathTest, DiesgetsockoptSocketError)
{
    // We are using mocks in a death test. Since the death test is run in a child process, the expectations
    // also need to be in the child process.
    EXPECT_EXIT(
        {
            // Arrange
            MockedFnsObj = std::make_shared<MockedFns>();

            int socketFd = 6;
            struct pollfd PollFd;
            PollFd.revents = POLLOUT;

            InSequence seq;
 
            EXPECT_CALL(*MockedFnsObj, llsocket)
            .Times(1)
            .WillOnce(Return(6));

            EXPECT_CALL(*MockedFnsObj, llconnect)
            .Times(1)
            .WillOnce(SetErrnoAndReturn(EINPROGRESS, -1));

            EXPECT_CALL(*MockedFnsObj, llpoll)
            .Times(1)
            .WillOnce(DoAll(SetArgPointee<0>(PollFd), Return(1)));

            int sysErrno = EACCES;

            EXPECT_CALL(*MockedFnsObj, llgetsockoptSocketError)
            .Times(1)
            .WillOnce(SetErrnoAndReturn(sysErrno, -1));

            // Act
            TcpSetConnectSocketNoBlock("localhost", "ftp", &socketFd);
        },
        // Assert
        ::testing::ExitedWithCode(1),
        AllOf(HasSubstr("code: " + std::to_string(SYSCALL_GETSOCKOPT)),
              HasSubstr("errno: " + std::to_string(EACCES) + " [" + strerror(EACCES) + "]")));
 
}

// -------------------------------------------------------------------------------------------
// One test to replace the previous 4 death tests. A bit more set up required. And probably not
// as readable as the individual tests. Retained as reference.
// -------------------------------------------------------------------------------------------

typedef struct
{
    int ret;
    int sysErrno;
    int errCode;
} reterr_t;
typedef struct
{
    constr_t fn;
    reterr_t failReterr;
    reterr_t okReterr;
} fnlist_t;

static fnlist_t FnList[] =
{
    // Function                     Failure                             Ok (success)
    { "llsocket",                   { -1, EACCES, SYSCALL_SOCKET },     { 6,    0,              0 } },
    { "llconnect",                  { -1, EACCES, SYSCALL_CONNECT },    { -1,   EINPROGRESS,    0 } },
    { "llpoll",                     { -1, EACCES, SYSCALL_POLL },       { 1,    0,              0 } },
    { "llgetsockoptSocketError",    { -1, EACCES, SYSCALL_GETSOCKOPT }, { 0,    0,              0 } },
};

static std::map<std::string, int> TcpSetConnectSocketRetryDeathTestD
{
    // Used to access the FnList[]. The call at max index is processed as failure; prior indices are processed as success. For e.g.
    // - llsocket has max index 0; this will be the only call processed (as failure).
    // - llconnect has max index 1; llsocket (at index 0) will be processed as success and llconnect will be processed as failure. 
    // - llpoll has max index 3; llsocket (at index 0) and llconnect (at index 1) will be processed as success and llpoll will be
    //   processed as failure. 
    // Function                     Max Index
    { "llsocket",                   0 },
    { "llconnect",                  1 },
    { "llpoll",                     2 },
    { "llgetsockoptSocketError",    3 },
};

class TcpSetConnectSocketRetryFDeathTest : public testing::TestWithParam<std::pair<const std::string, int>>
{

};

// Passed reterr can be either for failure or success.
static void addExpect(constr_t fn, reterr_t reterr)
{
    if (!strcmp(fn, "llsocket"))
    {
        // Expect for failure or success
        EXPECT_CALL(*MockedFnsObj, llsocket)
        .Times(1)
        .WillOnce(SetErrnoAndReturn(reterr.sysErrno, reterr.ret));
    }
    else if (!strcmp(fn, "llconnect"))
    {
        // Expect for failure or success
        EXPECT_CALL(*MockedFnsObj, llconnect)
        .Times(1)
        .WillOnce(SetErrnoAndReturn(reterr.sysErrno, reterr.ret));
    }
    else if (!strcmp(fn, "llpoll"))
    {
        if (reterr.ret == -1)
        {
            // Expect for failure
            EXPECT_CALL(*MockedFnsObj, llpoll)
            .Times(1)
            .WillOnce(SetErrnoAndReturn(reterr.sysErrno, reterr.ret));
        }
        else
        {
            // Expect for success (hard coded PollFd)
            struct pollfd PollFd;
            PollFd.revents = POLLOUT;
            EXPECT_CALL(*MockedFnsObj, llpoll)
            .Times(1)
            .WillOnce(DoAll(SetArgPointee<0>(PollFd), Return(reterr.ret)));
        }
    }
    else if (!strcmp(fn, "llgetsockoptSocketError"))
    {
        // Expect for failure (success not used since this is the last call in the function)
        EXPECT_CALL(*MockedFnsObj, llgetsockoptSocketError)
        .Times(1)
        .WillOnce(SetErrnoAndReturn(reterr.sysErrno, reterr.ret));
    }
}

static void procFnList(const int maxIdx)
{
    for (int i = 0; i <= maxIdx; i++)
    {
        if (i == maxIdx)
        {
            addExpect(FnList[i].fn, FnList[i].failReterr);
        }
        else
        {
            addExpect(FnList[i].fn, FnList[i].okReterr);
        }
    }
}

INSTANTIATE_TEST_SUITE_P(tcprtns, TcpSetConnectSocketRetryFDeathTest, testing::ValuesIn(TcpSetConnectSocketRetryDeathTestD),
[] (const testing::TestParamInfo<TcpSetConnectSocketRetryFDeathTest::ParamType>& info)
{
    return info.param.first;
});

TEST_P(TcpSetConnectSocketRetryFDeathTest, Dies)
{
    int maxIdx = GetParam().second;

    reterr_t failReterr = FnList[maxIdx].failReterr;
    // We are using mocks in a death test. Since the death test is run in a child process, the expectations
    // also need to be in the child process.
    EXPECT_EXIT(
        {
            // Arrange
            MockedFnsObj = std::make_shared<MockedFns>();

            int socketFd = 6;

            InSequence seq;
 
            // Add EXPECT_CALL based on maxIdx.
            procFnList(maxIdx);

            // Act
            TcpSetConnectSocketNoBlock("localhost", "ftp", &socketFd);
        },
        // Assert
        ::testing::ExitedWithCode(1),
        AllOf(HasSubstr("code: " + std::to_string(failReterr.errCode)),
              HasSubstr("errno: " + std::to_string(failReterr.sysErrno) + " [" + strerror(failReterr.sysErrno) + "]")));
}

TEST(retryLater, IsOk)
{
    // Arrange
    MockedFnsObj = std::make_shared<MockedFns>();

    int socketFd = 5;

    InSequence seq;

    EXPECT_CALL(*MockedFnsObj, llclose)
    .Times(1);

    // Act
    int ret = WRAPretryLater(&socketFd);

    // Assert
    EXPECT_EQ(-1, socketFd);
    EXPECT_EQ(1, ret);

    bool verified = Mock::VerifyAndClearExpectations(MockedFnsObj.get());
    EXPECT_TRUE(verified);
}

TEST(socketBlock, IsOk)
{
    // Arrange
    MockedFnsObj = std::make_shared<MockedFns>();

    int socketFd = 5;

    InSequence seq;

    EXPECT_CALL(*MockedFnsObj, llfcntl)
    .Times(2)
    .WillRepeatedly(Return(0));

    // Act
    int ret = WRAPsocketBlock(socketFd);

    // Assert
    EXPECT_EQ(5, socketFd);
    EXPECT_EQ(0, ret);

    bool verified = Mock::VerifyAndClearExpectations(MockedFnsObj.get());
    EXPECT_TRUE(verified);
}

TEST(socketBlockDeathTest, DiesFcntl1)
{
    // We are using mocks in a death test. Since the death test is run in a child process, the expectations
    // also need to be in the child process.
    EXPECT_EXIT(
        {
            // Arrange
            MockedFnsObj = std::make_shared<MockedFns>();

            int socketFd = 6;

            InSequence seq;

            int sysErrno = EACCES;

            EXPECT_CALL(*MockedFnsObj, llfcntl)
            .Times(1)
            .WillOnce(SetErrnoAndReturn(sysErrno, -1));

            // Act
            WRAPsocketBlock(socketFd);
        },
        // Assert
        ::testing::ExitedWithCode(1),
        AllOf(HasSubstr("code: " + std::to_string(SYSCALL_FCNTL)),
              HasSubstr("errno: " + std::to_string(EACCES) + " [" + strerror(EACCES) + "]")));
}

TEST(socketBlockDeathTest, DiesFcntl2)
{
    // We are using mocks in a death test. Since the death test is run in a child process, the expectations
    // also need to be in the child process.
    EXPECT_EXIT(
        {
            // Arrange
            MockedFnsObj = std::make_shared<MockedFns>();

            int socketFd = 6;

            InSequence seq;

            int sysErrno = EACCES;

            EXPECT_CALL(*MockedFnsObj, llfcntl)
            .Times(1)
            .WillOnce(Return(0));

            EXPECT_CALL(*MockedFnsObj, llfcntl)
            .Times(1)
            .WillOnce(SetErrnoAndReturn(sysErrno, -1));

            // Act
            WRAPsocketBlock(socketFd);
        },
        // Assert
        ::testing::ExitedWithCode(1),
        AllOf(HasSubstr("code: " + std::to_string(SYSCALL_FCNTL)),
              HasSubstr("errno: " + std::to_string(EACCES) + " [" + strerror(EACCES) + "]")));
}

TEST(TcpShutdown, IsOk)
{
    // Arrange
    MockedFnsObj = std::make_shared<MockedFns>();

    int socketFd = 5;
    struct pollfd PollFd;
    PollFd.revents = POLLIN;

    InSequence seq;

    EXPECT_CALL(*MockedFnsObj, llshutdown(_, SHUT_WR))
    .Times(1);

    EXPECT_CALL(*MockedFnsObj, llpoll)
    .Times(1)
    .WillOnce(DoAll(SetArgPointee<0>(PollFd), Return(1)));

    EXPECT_CALL(*MockedFnsObj, llrecv)
    .Times(2)
    .WillOnce(Return(10))
    .WillOnce(Return(0));

    EXPECT_CALL(*MockedFnsObj, llshutdown(_, SHUT_RD))
    .Times(1);

    EXPECT_CALL(*MockedFnsObj, llclose)
    .Times(1);

    // Act
    TcpShutdown(&socketFd);

    // Assert
    EXPECT_EQ(-1, socketFd);

    bool verified = Mock::VerifyAndClearExpectations(MockedFnsObj.get());
    EXPECT_TRUE(verified);
}
