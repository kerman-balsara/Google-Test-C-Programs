// ---------------------
// External definitions.
// ---------------------

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using ::testing::Mock;
using ::testing::DoAll;
using ::testing::HasSubstr;
using ::testing::InSequence;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::SetArrayArgument;
using ::testing::SetErrnoAndReturn;

#include <errno.h>

#include <poll.h>
#include <string.h>

// ---------------------
// Internal definitions.
// ---------------------

#include <mockedfns.hpp>

#include <errcodes.h>

#include <memrtns.h>
#include <wrapsocketrecv.h>

#define LLRECV_MAX_CALLS 3
#define READBUF_MAX 50
#define LLRECV_MAX_MESSAGES 5

// Test data for llrecv
typedef struct
{
    int startReadByte;
    int endReadByte;        // end of the source range (exclusive). 
    int bytesRead;
} recvbuf_t;

typedef struct
{
    int maxRecvCalls;       // Cannot exceed LLRECV_MAX_CALLS
    char readBuf[READBUF_MAX];
    recvbuf_t recvBuf[LLRECV_MAX_CALLS];
    constr_t msglenInd;

    int expMaxMsg;          // Cannot exceed LLRECV_MAX_MESSAGES
    char expMsg[LLRECV_MAX_MESSAGES][READBUF_MAX];
    int expMsgLen[LLRECV_MAX_MESSAGES];
} bcdata_t;

static bcdata_t ByClientData[] = 
{
    // Test case for messages with a terminating null.
    // We expect llrecv to be called 1 time. The entire string specified in readBuf will be populated
    // by that call. However, we expect 5 complete messages to be obtained by that call.
    { 1, "ABCDEF\0GHIJ\0KLMNO\0PQRSTUV\0WXYZ\0", { { 0, 32, 31 } }, "N", 
        5, { "ABCDEF\0", "GHIJ\0", "KLMNO\0", "PQRSTUV\0", "WXYZ\0" }, { 7, 5, 6, 8, 5 } },
    // Test case for messages with a terminating null.
    // We expect llrecv to be called 3 times each time with a different subsection of readBuf. However, we
    // expect only 1 complete message to be obtained.
    { 3, "ABCDEFGHIJKLMNOPQRSTUVWXYZ\0", { { 0, 10, 10 }, { 10, 20, 10 }, { 20, 27, 7 } }, "N", 
        1, { "ABCDEFGHIJKLMNOPQRSTUVWXYZ\0" }, { 27 } },

    // Test case for messages with a length in the message itself.
    // We expect llrecv to be called 1 time. The entire string specified in readBuf will be populated
    // by that call. However, we expect 3 complete messages to be obtained by that call.
    // Note the hex values for the length field (2 bytes for short). The length does not include the length field itself.
    // 0x0006 is length 6 for "ABCDEF", 0x0009 is length 9 for "GHIJKLMNO" and 0x000B is length 11 for "PQRSTUVWXYZ". 
    { 1, "\x00" "\x06" "ABCDEF" "\x00" "\x09" "GHIJKLMNO" "\x00" "\x0B" "PQRSTUVWXYZ", { 0, 32, 32 }, "M", 
        3, { "\x00" "\x06" "ABCDEF", "\x00" "\x09" "GHIJKLMNO", "\x00" "\x0B" "PQRSTUVWXYZ" },  { 8, 11, 13 } },
    // Test case for messages with a length in the message itself.
    // We expect llrecv to be called 3 times each time with a different subsection of readBuf. However, we
    // expect only 1 complete message to be obtained.
    { 3, "\x00" "\x1A" "ABCDEFGHIJKLMNOPQRSTUVWXYZ", { { 0, 12, 12 }, { 12, 22, 10 }, { 22, 28, 6 } }, "M", 
        1, { "\x00" "\x1A" "ABCDEFGHIJKLMNOPQRSTUVWXYZ" }, { 28 } },

    // Test case for messages with a length in the message itself.
    // We expect llrecv to be called 1 time. The entire string specified in readBuf will be populated
    // by that call. However, we expect 3 complete messages to be obtained by that call.
    // Note the hex values for the length field (2 bytes for short). The length includes the length field.
    // 0x0008 is length 8 for "ABCDEF" + 2 for length field, 0x000B is length 11 for "GHIJKLMNO" + 2 for length field
    // and 0x000D is length 13 for "PQRSTUVWXYZ" + 2 for length field. 
    { 1, "\x00" "\x08" "ABCDEF" "\x00" "\x0B" "GHIJKLMNO" "\x00" "\x0D" "PQRSTUVWXYZ", { 0, 32, 32 }, "T", 
        3, { "\x00" "\x08" "ABCDEF", "\x00" "\x0B" "GHIJKLMNO", "\x00" "\x0D" "PQRSTUVWXYZ" },  { 8, 11, 13 } },
    // Test case for messages with a length in the message itself.
    // We expect llrecv to be called 3 times each time with a different subsection of readBuf. However, we
    // expect only 1 complete message to be obtained.
    { 3, "\x00" "\x1C" "ABCDEFGHIJKLMNOPQRSTUVWXYZ", { { 0, 12, 12 }, { 12, 22, 10 }, { 22, 28, 6 } }, "T", 
        1, { "\x00" "\x1C" "ABCDEFGHIJKLMNOPQRSTUVWXYZ" }, { 28 } },
};

static std::map<std::string, bcdata_t> ByClientD
{
    { "WithIndN", ByClientData[0] },
    { "WithIndNMultiRecv", ByClientData[1] },

    { "WithIndM", ByClientData[2] },
    { "WithIndMMultiRecv", ByClientData[3] },

    { "WithIndT", ByClientData[4] },
    { "WithIndTMultiRecv", ByClientData[5] },
};

class SocketRecvByClientF : public testing::TestWithParam<std::pair<const std::string, bcdata_t>>
{
};

INSTANTIATE_TEST_SUITE_P(byclient, SocketRecvByClientF, testing::ValuesIn(ByClientD),
[] (const testing::TestParamInfo<SocketRecvByClientF::ParamType>& info)
{
    return info.param.first;
});

TEST_P(SocketRecvByClientF, IsOk)
{
    // Arrange
    int fd = 99;

    char * msg = NULL;
    char * prevMsg = NULL;
    int msgLen = 0;
    int prevMsgLen = 0;

    constr_t mapKey = strchr(testing::UnitTest::GetInstance()->current_test_info()->name(), '/') + 1;

    bcdata_t bcdata = GetParam().second;

    struct pollfd PollFd;
    PollFd.revents = POLLIN;    

    {
        MockedFnsObj = std::make_shared<MockedFns>();

        InSequence seq;
 
        for (int i = 0; i < bcdata.maxRecvCalls; i++)
        {
            EXPECT_CALL(*MockedFnsObj, llpoll)
            .Times(1)
            .WillOnce(DoAll(SetArgPointee<0>(PollFd), Return(1)));

            EXPECT_CALL(*MockedFnsObj, llrecv)
            .Times(1)
            .WillOnce(DoAll(SetArrayArgument<1>(&bcdata.readBuf[bcdata.recvBuf[i].startReadByte],
                                                &bcdata.readBuf[bcdata.recvBuf[i].endReadByte]),
                            Return(bcdata.recvBuf[i].bytesRead)));
        }
    }
 
    // Act
    for (int i = 0; i < bcdata.expMaxMsg; i++)
    {
        int ret = SocketRecvByClient(mapKey, fd, bcdata.msglenInd, &msg, &msgLen, &prevMsg, &prevMsgLen);

        EXPECT_EQ(0, ret);
        EXPECT_EQ(bcdata.expMsgLen[i], msgLen);
        EXPECT_EQ(0, memcmp(bcdata.expMsg[i], msg, msgLen));

        bool verified = Mock::VerifyAndClearExpectations(MockedFnsObj.get());
        EXPECT_TRUE(verified);

        MemFree(mapKey, &msg);
        msgLen = 0;
    }    

    EXPECT_EQ(0, prevMsgLen);
    MemFree(mapKey, &prevMsg);
}

typedef struct
{
    int llpollRevent;
    int llpollErrno;
    int llpollRet;

    // Call llrecv only if llpoll succeeds
    bool_t llpollSuccess;

    int llrecvErrno;
    int llrecvRet;

    int expErrno;
    int expErrCode;
} bcdtdata_t;

static bcdtdata_t ByClientDeathTestData[] = 
{
    { POLLIN,  EACCES, -1, 0, 0,       0, EACCES, SYSCALL_POLL },
    { POLLERR, 0,       1, 0, 0,       0, 0,      SYSCALL_POLL },
    { POLLIN,  0,       1, 1, EACCES, -1, EACCES, SYSCALL_RECV },
};

static std::map<std::string, bcdtdata_t> ByClientDeathTestD
{
    { "PollErrno", ByClientDeathTestData[0] },
    { "PollErr", ByClientDeathTestData[1] },
    { "RecvErrno", ByClientDeathTestData[2] },
};

class SocketRecvByClientFDeathTest : public testing::TestWithParam<std::pair<const std::string, bcdtdata_t>>
{
};

INSTANTIATE_TEST_SUITE_P(byclientdt, SocketRecvByClientFDeathTest, testing::ValuesIn(ByClientDeathTestD),
[] (const testing::TestParamInfo<SocketRecvByClientFDeathTest::ParamType>& info)
{
    return info.param.first;
});

TEST_P(SocketRecvByClientFDeathTest, Dies)
{

    bcdtdata_t bcdtdata = GetParam().second;
    string_t expStderr[50];
    if (bcdtdata.expErrno == 0)
    {
        snprintf(expStderr, sizeof(expStderr), ".*code: %d.*", bcdtdata.expErrCode);
    }
    else
    {
        snprintf(expStderr, sizeof(expStderr), ".*code: %d.*errno: %d.*",
            bcdtdata.expErrCode, bcdtdata.expErrno);
    }

    // We are using mocks in a death test. Since the death test is run in a child process, the expectations
    // also need to be in the child process.
    EXPECT_EXIT(
        {
            // Arrange
            MockedFnsObj = std::make_shared<MockedFns>();

            int fd = 99;

            char * msg = NULL;
            char * prevMsg = NULL;
            int msgLen = 0;
            int prevMsgLen = 0;

            struct pollfd PollFd;
            PollFd.revents = bcdtdata.llpollRevent;    

            InSequence seq;

            EXPECT_CALL(*MockedFnsObj, llpoll)
            .Times(1)
            .WillOnce(DoAll(SetArgPointee<0>(PollFd), SetErrnoAndReturn(bcdtdata.llpollErrno, bcdtdata.llpollRet)));

            if (bcdtdata.llpollSuccess)
            {
                EXPECT_CALL(*MockedFnsObj, llrecv)
                .Times(1)
                .WillOnce(SetErrnoAndReturn(bcdtdata.llrecvErrno, bcdtdata.llrecvRet));
            }

            // Act
            SocketRecvByClient("SocketRecvByClientFDeathTest", fd, "N", &msg, &msgLen, &prevMsg, &prevMsgLen);
        },
        ::testing::ExitedWithCode(1),
        expStderr);
}

TEST(SocketRecvByClient, IsOkPollValidErrnoRecvDisconnect)
{
    // Arrange
    MockedFnsObj = std::make_shared<MockedFns>();

    int fd = 99;

    char * msg = NULL;
    char * prevMsg = NULL;
    int msgLen = 0;
    int prevMsgLen = 0;

    struct pollfd PollFd;
    PollFd.revents = POLLIN;
    
    int maxItr = 3;
    int llpollRet[maxItr] = {    -1,     -1, 1 };
    int sysErrno[maxItr]  = { EINTR, EAGAIN, 0 };

    InSequence seq;

    for (int i = 0; i < maxItr; i++)
    {
        EXPECT_CALL(*MockedFnsObj, llpoll)
        .Times(1)
        .WillOnce(DoAll(SetArgPointee<0>(PollFd), SetErrnoAndReturn(sysErrno[i], llpollRet[i])));
        if (i == 2)
        {
            EXPECT_CALL(*MockedFnsObj, llrecv)
            .Times(1)
            .WillOnce(Return(0));
        }
    }

    // Act
    int ret = SocketRecvByClient("IsOkPollValidErrnoRecvDisconnect", fd, "N", &msg, &msgLen, &prevMsg, &prevMsgLen);

    EXPECT_EQ(1, ret);

    bool verified = Mock::VerifyAndClearExpectations(MockedFnsObj.get());
    EXPECT_TRUE(verified);
}

TEST(SocketRecvByClient, IsOkRecvValidErrnoRecvDisconnect)
{
    // Arrange
    MockedFnsObj = std::make_shared<MockedFns>();

    int fd = 99;

    char * msg = NULL;
    char * prevMsg = NULL;
    int msgLen = 0;
    int prevMsgLen = 0;

    struct pollfd PollFd;
    PollFd.revents = POLLIN;
    
    int maxItr = 3;
    int llrecvRet[maxItr] = {    -1,     -1, 0 };
    int sysErrno[maxItr]  = { EINTR, EAGAIN, 0 };

    InSequence seq;

    for (int i = 0; i < maxItr; i++)
    {
        EXPECT_CALL(*MockedFnsObj, llpoll)
        .Times(1)
        .WillOnce(DoAll(SetArgPointee<0>(PollFd), Return(1)));
        EXPECT_CALL(*MockedFnsObj, llrecv)
        .Times(1)
        .WillOnce(SetErrnoAndReturn(sysErrno[i], llrecvRet[i]));
    }

    // Act
    int ret = SocketRecvByClient("IsOkRecvValidErrnoRecvDisconnect", fd, "N", &msg, &msgLen, &prevMsg, &prevMsgLen);

    EXPECT_EQ(1, ret);

    bool verified = Mock::VerifyAndClearExpectations(MockedFnsObj.get());
    EXPECT_TRUE(verified);
}

TEST(SocketRecvByClient, IsOkRecvConnReset)
{
    // Arrange
    MockedFnsObj = std::make_shared<MockedFns>();

    int fd = 99;

    char * msg = NULL;
    char * prevMsg = NULL;
    int msgLen = 0;
    int prevMsgLen = 0;

    struct pollfd PollFd;
    PollFd.revents = POLLIN;
    
    InSequence seq;

    EXPECT_CALL(*MockedFnsObj, llpoll)
    .Times(1)
    .WillOnce(DoAll(SetArgPointee<0>(PollFd), Return(1)));
    EXPECT_CALL(*MockedFnsObj, llrecv)
    .Times(1)
    .WillOnce(SetErrnoAndReturn(ECONNRESET, -1));
 
    // Act
    int ret = SocketRecvByClient("IsOkRecvConnReset", fd, "N", &msg, &msgLen, &prevMsg, &prevMsgLen);

    EXPECT_EQ(1, ret);

    bool verified = Mock::VerifyAndClearExpectations(MockedFnsObj.get());
    EXPECT_TRUE(verified);
}

TEST(SocketRecv, IsOkRecvConnReset)
{
    // Arrange
    MockedFnsObj = std::make_shared<MockedFns>();

    int fd = 99;

    char * prevMsg = NULL;
    int prevMsgLen = 0;
    bool_t msgComplete;

    InSequence seq;

    EXPECT_CALL(*MockedFnsObj, llrecv)
    .Times(1)
    .WillOnce(SetErrnoAndReturn(ECONNRESET, -1));

    // Act
    int actRet = SocketRecv("IsOkRecvConnReset", fd, "N", &prevMsg, &prevMsgLen, &msgComplete);

    EXPECT_EQ(actRet, 1);

    bool verified = Mock::VerifyAndClearExpectations(MockedFnsObj.get());
    EXPECT_TRUE(verified);
}

TEST(SocketRecv, IsOkRecvValidErrnoRecvDisconnect)
{
    // Arrange
    MockedFnsObj = std::make_shared<MockedFns>();

    int fd = 99;

    char * prevMsg = NULL;
    int prevMsgLen = 0;
    bool_t msgComplete;

    int maxItr = 3;
    int sysErrno[maxItr] = { EINTR, EAGAIN, 0 };
    int llrecvRet[maxItr] = { -1, -1, 0 };

    InSequence seq;

    for (int i = 0; i < maxItr; i++)
    {
        EXPECT_CALL(*MockedFnsObj, llrecv)
        .Times(1)
        .WillOnce(SetErrnoAndReturn(sysErrno[i], llrecvRet[i]));
    }

    // Act
    int actRet = SocketRecv("IsOkRecvValidErrnoRecvDisconnect", fd, "N", &prevMsg, &prevMsgLen, &msgComplete);

    EXPECT_EQ(actRet, 1);

    bool verified = Mock::VerifyAndClearExpectations(MockedFnsObj.get());
    EXPECT_TRUE(verified);
}

TEST(SocketRecvDeathTest, Diesrecv)
{
    // We are using mocks in a death test. Since the death test is run in a child process, the expectations
    // also need to be in the child process.
    EXPECT_EXIT(
        {
            // Arrange
            MockedFnsObj = std::make_shared<MockedFns>();

            int fd = 99;

            char * prevMsg = NULL;
            int prevMsgLen = 0;
            bool_t msgComplete;

            int sysErrno = EACCES;

            InSequence seq;

            EXPECT_CALL(*MockedFnsObj, llrecv)
            .Times(1)
            .WillOnce(SetErrnoAndReturn(sysErrno, -1));

            // Act
            SocketRecv("SocketRecvDeathTest", fd, "N", &prevMsg, &prevMsgLen, &msgComplete);
        },
        ::testing::ExitedWithCode(1),
        AllOf(HasSubstr("code: " + std::to_string(SYSCALL_RECV)),
              HasSubstr("errno: " + std::to_string(EACCES) + " [" + strerror(EACCES) + "]")));
}

typedef struct
{
    int maxRecvCalls;       // Cannot exceed LLRECV_MAX_CALLS
    char readBuf[READBUF_MAX];
    recvbuf_t recvBuf[LLRECV_MAX_CALLS];
    constr_t msglenInd;
    int expMsgComplete[LLRECV_MAX_CALLS];

    char expMsg[READBUF_MAX];
    int expMsgLen;

} ydata_t;

static ydata_t YieldData[] = 
{
    // Test case for messages with a terminating null.
    // We expect llrecv to be called 1 time. The entire string specified in readBuf will be populated
    // by that call. At the end, we expect 1 complete message to be obtained.
    { 1, "ABCDEF\0", { { 0, 8, 7 } }, "N", { 1 },
        "ABCDEF\0", 7 },
    // Test case for messages with a terminating null.
    // We expect llrecv to be called 3 times each time with a different subsection of readBuf. At the end,
    // we expect 1 complete message to be obtained.
    { 3, "ABCDEFGHIJKLMNOPQRSTUVWXYZ\0", { { 0, 10, 10 }, { 10, 20, 10 }, { 20, 27, 7 } }, "N", { 0, 0, 1 },
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ\0", 27 },

    // Test case for messages with a length in the message itself.
    // We expect llrecv to be called 1 time. The entire string specified in readBuf will be populated
    // by that call. At the end, we expect 1 complete message to be obtained.
    // Note the hex values for the length field (2 bytes for short). The length does not include the length field itself.
    // 0x0006 is length 6 for "ABCDEF". 
    { 1, "\x00" "\x06" "ABCDEF", { 0, 9, 8 }, "M", { 1 },
        "\x00" "\x06" "ABCDEF", 8 },
    // Test case for messages with a length in the message itself.
    // We expect llrecv to be called 3 times each time with a different subsection of readBuf.
    // At the end, we expect 1 complete message to be obtained. 
    { 3, "\x00" "\x1A" "ABCDEFGHIJKLMNOPQRSTUVWXYZ", { { 0, 12, 12 }, { 12, 22, 10 }, { 22, 28, 6 } }, "M", { 0, 0, 1 }, 
        "\x00" "\x1A" "ABCDEFGHIJKLMNOPQRSTUVWXYZ", 28 },

    // Test case for messages with a length in the message itself.
    // We expect llrecv to be called 1 time. The entire string specified in readBuf will be populated
    // by that call. At the end, we expect 1 complete message to be obtained.
    // Note the hex values for the length field (2 bytes for short). The length includes the length field.
    // 0x0008 is length 8 for "ABCDEF" + 2 for length field. 
    { 1, "\x00" "\x08" "ABCDEF", { 0, 9, 8 }, "T", { 1 }, 
        "\x00" "\x08" "ABCDEF", 8 },
    // Test case for messages with a length in the message itself.
    // We expect llrecv to be called 3 times each time with a different subsection of readBuf.
    // At the end, we expect 1 complete message to be obtained. 
    { 3, "\x00" "\x1C" "ABCDEFGHIJKLMNOPQRSTUVWXYZ", { { 0, 12, 12 }, { 12, 22, 10 }, { 22, 28, 6 } }, "T", { 0, 0, 1 }, 
        "\x00" "\x1C" "ABCDEFGHIJKLMNOPQRSTUVWXYZ", 28 },
};

static std::map<std::string, ydata_t> YieldD
{
    { "WithIndN", YieldData[0] },
    { "WithIndNMultiRecv", YieldData[1] },

    { "WithIndM", YieldData[2] },
    { "WithIndMMultiRecv", YieldData[3] },

    { "WithIndT", YieldData[4] },
    { "WithIndTMultiRecv", YieldData[5] },
};

class SocketRecvYieldF : public testing::TestWithParam<std::pair<const std::string, ydata_t>>
{
};

INSTANTIATE_TEST_SUITE_P(yield, SocketRecvYieldF, testing::ValuesIn(YieldD),
[] (const testing::TestParamInfo<SocketRecvYieldF::ParamType>& info)
{
    return info.param.first;
});

TEST_P(SocketRecvYieldF, IsOk)
{
    // Arrange
    int fd = 99;

    char * prevMsg = NULL;
    int prevMsgLen = 0;
    bool_t msgComplete;

    constr_t mapKey = strchr(testing::UnitTest::GetInstance()->current_test_info()->name(), '/') + 1;

    ydata_t ydata = GetParam().second;

    {
        MockedFnsObj = std::make_shared<MockedFns>();

        InSequence seq;
 
        for (int i = 0; i < ydata.maxRecvCalls; i++)
        {
            EXPECT_CALL(*MockedFnsObj, llrecv)
            .Times(1)
            .WillOnce(DoAll(SetArrayArgument<1>(&ydata.readBuf[ydata.recvBuf[i].startReadByte],
                                                &ydata.readBuf[ydata.recvBuf[i].endReadByte]),
                            Return(ydata.recvBuf[i].bytesRead)));

            int actRet = SocketRecv(mapKey, fd, ydata.msglenInd, &prevMsg, &prevMsgLen, &msgComplete);

            EXPECT_EQ(0, actRet);
            EXPECT_EQ(ydata.expMsgComplete[i], msgComplete);

            bool verified = Mock::VerifyAndClearExpectations(MockedFnsObj.get());
            EXPECT_TRUE(verified);
        }

    }
 
    EXPECT_EQ(ydata.expMsgLen, prevMsgLen);
    EXPECT_EQ(0, memcmp(ydata.expMsg, prevMsg, prevMsgLen));
    MemFree(mapKey, &prevMsg);
    prevMsgLen = 0;
}

