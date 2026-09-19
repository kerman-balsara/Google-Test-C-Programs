// ---------------------
// External definitions.
// ---------------------

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using ::testing::InSequence;
using ::testing::Mock;
using ::testing::Return;
using ::testing::SetErrnoAndReturn;

#include <errno.h>

#include <stdio.h>
#include <string.h>

// ---------------------
// Internal definitions.
// ---------------------

#include <mockedfns.hpp>

#include <errcodes.h>

#include <llsocket.h>
#include <wrapsocketsend.h>

static std::map<std::string, std::tuple<const int, const int>> DeathTestD
{
    // Tuple: llsend() errno, llsend() return code
    {"FailErrno", std::make_tuple(EACCES, -1)},
    {"FailBytesZero", std::make_tuple(0, 0)},
};

class SocketSendNoYieldFDeathTest : public testing::TestWithParam<std::pair<const std::string, std::tuple<const int, const int>>>
{
};

INSTANTIATE_TEST_SUITE_P(noyielddt, SocketSendNoYieldFDeathTest, testing::ValuesIn(DeathTestD),
[] (const testing::TestParamInfo<SocketSendNoYieldFDeathTest::ParamType>& info)
{
    return info.param.first;
});

TEST_P(SocketSendNoYieldFDeathTest, Dies)
{
    // Arrange
    std::tuple<const int, const int> tuple = GetParam().second;
    const int sysErrno = std::get<0>(tuple);
    const int llsendRet = std::get<1>(tuple);
    
    string_t expStderr[50];
    if (sysErrno == 0)
    {
        snprintf(expStderr, sizeof(expStderr), ".*code: %d.*", SYSCALL_SEND);
    }
    else
    {
        snprintf(expStderr, sizeof(expStderr), ".*code: %d.*errno: %d.*", SYSCALL_SEND, sysErrno);
    }
    // We are using mocks in a death test. Since the death test is run in a child process, the expectations
    // also need to be in the child process.
    EXPECT_EXIT(
        {
            // Arrange
            MockedFnsObj = std::make_shared<MockedFns>();

            int fd = 5;

            char buf[20];
            int buflen = 5;

            InSequence seq;
        
            EXPECT_CALL(*MockedFnsObj, llsend)
            .Times(1)
            .WillOnce(SetErrnoAndReturn(sysErrno, llsendRet));

            // Act
            SocketSendNoYield(fd, buf, buflen);
        },
        ::testing::ExitedWithCode(1),
        expStderr);
}

TEST(SocketSendNoYield, IsOk)
{
    // Arrange
    int fd = 5;
    char buf[30];
    int buflen = 30;

    {
        MockedFnsObj = std::make_shared<MockedFns>();

        InSequence seq;
    
        EXPECT_CALL(*MockedFnsObj, llsend)
        .Times(3)
        .WillRepeatedly(Return(10));
    }
 
    // Act
    int actRet = SocketSendNoYield(fd, buf, buflen);
    EXPECT_EQ(0, actRet);

    bool verified = Mock::VerifyAndClearExpectations(MockedFnsObj.get());
    EXPECT_TRUE(verified);
}

TEST(SocketSendNoYield, IsOkValidErrno)
{
    // Arrange
    MockedFnsObj = std::make_shared<MockedFns>();

    int fd = 5;
    char buf[30];
    int buflen = 30;

    {
        InSequence seq;
    
        EXPECT_CALL(*MockedFnsObj, llsend)
        .Times(5)
        .WillOnce(SetErrnoAndReturn(0, 10))
        .WillOnce(SetErrnoAndReturn(0, 10))
        .WillOnce(SetErrnoAndReturn(EINTR, -1))
        .WillOnce(SetErrnoAndReturn(EAGAIN, -1))
        .WillOnce(SetErrnoAndReturn(0, 10));
    }
 
    // Act
    int actRet = SocketSendNoYield(fd, buf, buflen);
    EXPECT_EQ(0, actRet);

    bool verified = Mock::VerifyAndClearExpectations(MockedFnsObj.get());
    EXPECT_TRUE(verified);
}

TEST(SocketSendNoYield, IsOkDisconnect)
{
    // Arrange
    MockedFnsObj = std::make_shared<MockedFns>();

    int fd = 5;
    char buf[30];
    int buflen = 30;

    {
        InSequence seq;
    
        EXPECT_CALL(*MockedFnsObj, llsend)
        .Times(3)
        .WillOnce(SetErrnoAndReturn(0, 10))
        .WillOnce(SetErrnoAndReturn(0, 10))
        .WillOnce(SetErrnoAndReturn(ECONNRESET, -1));
    }
 
    // Act
    int actRet = SocketSendNoYield(fd, buf, buflen);
    EXPECT_EQ(1, actRet);

    bool verified = Mock::VerifyAndClearExpectations(MockedFnsObj.get());
    EXPECT_TRUE(verified);
}

class SocketSendYieldFDeathTest : public testing::TestWithParam<std::pair<const std::string, std::tuple<const int, const int>>>
{
};

INSTANTIATE_TEST_SUITE_P(yielddt, SocketSendYieldFDeathTest, testing::ValuesIn(DeathTestD),
[] (const testing::TestParamInfo<SocketSendYieldFDeathTest::ParamType>& info)
{
    return info.param.first;
});

TEST_P(SocketSendYieldFDeathTest, Diessend)
{
    // Arrange
    std::tuple<const int, const int> tuple = GetParam().second;
    const int sysErrno = std::get<0>(tuple);
    const int llsendRet = std::get<1>(tuple);
    
    string_t expStderr[50];
    if (sysErrno == 0)
    {
        snprintf(expStderr, sizeof(expStderr), ".*code: %d.*", SYSCALL_SEND);
    }
    else
    {
        snprintf(expStderr, sizeof(expStderr), ".*code: %d.*errno: %d.*", SYSCALL_SEND, sysErrno);
    }
    // We are using mocks in a death test. Since the death test is run in a child process, the expectations
    // also need to be in the child process.
    EXPECT_EXIT(
        {
            // Arrange
            MockedFnsObj = std::make_shared<MockedFns>();

            int fd = 5;
            char buf[20];
            int buflen = 5;
            int prevBytesSent = 0;

            InSequence seq;
        
            EXPECT_CALL(*MockedFnsObj, llsend)
            .Times(1)
            .WillOnce(SetErrnoAndReturn(sysErrno, llsendRet));

            // Act
            SocketSendYield(fd, buf, buflen, &prevBytesSent);
        },
        ::testing::ExitedWithCode(1),
        expStderr);
}

TEST(SocketSendYield, IsOkValidErrnoAndComplete)
{
    // Arrange
    MockedFnsObj = std::make_shared<MockedFns>();

    int fd = 5;
    char buf[20];
    int buflen = 5;
    int prevBytesSent = 0;

    int maxItr = 3;
    int llsendRet[maxItr] = { -1,    -1,     5 };
    int sysErrno[maxItr]  = { EINTR, EAGAIN, 0 };

    InSequence seq;

    for (int i = 0; i < maxItr; i++)
    {
        EXPECT_CALL(*MockedFnsObj, llsend)
        .Times(1)
        .WillOnce(SetErrnoAndReturn(sysErrno[i], llsendRet[i]));
    }

    // Act
    int ret = SocketSendYield(fd, buf, buflen, &prevBytesSent);

    EXPECT_EQ(0, ret);
    EXPECT_EQ(5, prevBytesSent);

    bool verified = Mock::VerifyAndClearExpectations(MockedFnsObj.get());
    EXPECT_TRUE(verified);
}

TEST(SocketSendYield, IsOkSendPartialAndComplete)
{
    // Arrange
    MockedFnsObj = std::make_shared<MockedFns>();

    int fd = 5;
    char buf[20];
    int buflen = 5;
    int prevBytesSent = 0; 

    int maxItr = 2;
    int llsendRet[maxItr]        = { 3, 2 };
    int expPrevBytesSent[maxItr] = { 3, 5 };
    int expRet[maxItr]           = { 1, 0 }; 

    InSequence seq;

    for (int i = 0; i < maxItr; i++)
    {
        EXPECT_CALL(*MockedFnsObj, llsend)
        .Times(1)
        .WillOnce(Return(llsendRet[i]));

        // Act
        int ret = SocketSendYield(fd, buf, buflen, &prevBytesSent);

        EXPECT_EQ(expRet[i], ret);
        EXPECT_EQ(expPrevBytesSent[i], prevBytesSent);
    }

    EXPECT_EQ(5, prevBytesSent);

    bool verified = Mock::VerifyAndClearExpectations(MockedFnsObj.get());
    EXPECT_TRUE(verified);
}

