#include <gtest/gtest.h>
#include <gmock/gmock.h>

using ::testing::HasSubstr;

#include <errcodes.h>

#include <memrtns.h>
#include <msglenrtns.h>

TEST(MsgStartPos, IsOk)
{
    // Arrange
    char msglenInd[3] = { 'N', 'T', 'M'};
    int expStartPos[3] = { 0, 2 , 2};

    // Act & Assert
    for (int i = 0; i < 3; i++)
    {
        int actStartPos = MsgStartPos(&msglenInd[i]);
        EXPECT_EQ(expStartPos[i], actStartPos) << "MsgStartPos failed for indicator " << msglenInd[i];
    }
}

TEST(MsgLenFromXmitLen, IsOk)
{
    // Arrange
    char msglenInd[3] = { 'N', 'T', 'M'};
    int xmitLen = 25;
    int expMsgLen[3] = { 24, 23, 23};

    // Act & Assert
    for (int i = 0; i < 3; i++)
    {
        int actMsgLen = MsgLenFromXmitLen(&msglenInd[i], xmitLen);
        EXPECT_EQ(expMsgLen[i], actMsgLen) << "MsgLenFromXmitLen failed for indicator " << msglenInd[i];
    }
}

TEST(MsgLenToXmitLen, IsOk)
{
    // Arrange
    char msglenInd[3] = { 'N', 'T', 'M'};
    int msgLen = 25;
    int expXmitLen[3] = { 26, 27, 27};

    // Act & Assert
    for (int i = 0; i < 3; i++)
    {
        int actXmitLen = MsgLenToXmitLen(&msglenInd[i], msgLen);
        EXPECT_EQ(expXmitLen[i], actXmitLen) << "MsgLenToXmitLen failed for indicator " << msglenInd[i];
    }
}

TEST(MsgLenToBuf, IsOk)
{
    // Arrange
    char msglenInd[2] = { 'T', 'M'};
    int msgLen = 25;
    char expBuf[2][2] = { '\x00', '\x1B', '\x00', '\x19'};

    char buf[2];

    // Act & Assert
    for (int i = 0; i < 2; i++)
    {
        MsgLenToBuf(&msglenInd[i], msgLen, buf);
        EXPECT_EQ(expBuf[i][0], buf[0]) << "MsgLenToBuf (0) failed for indicator " << msglenInd[i];
        EXPECT_EQ(expBuf[i][1], buf[1]) << "MsgLenToBuf (1) failed for indicator " << msglenInd[i];
    }
}

TEST(MsgLenToBufDeathTest, DiesMsgLenIndInvalid)
{
    // Arrange
    char buf[2];

    // Act & Assert
    EXPECT_EXIT(MsgLenToBuf("N", 25, buf),
                ::testing::ExitedWithCode(1),
                HasSubstr("code: " + std::to_string(MSGLEN_IND_INVALID)));
}

#define READBUF_MAX 50
#define MAX_MESSAGES 5

typedef struct
{
    char readBuf[READBUF_MAX];
    int bytesRead;
    constr_t msglenInd;

    int expMaxMsg;          // Cannot exceed MAX_MESSAGES
    char expMsg[MAX_MESSAGES][READBUF_MAX];
    int expMsgLen[MAX_MESSAGES];

    bool_t expMsgComplete[MAX_MESSAGES];
} mgcdata_t;

// Note1: We cannot have 2 consecutive incomplete messages
// Note2: An incomplete message can only be at the end i.e. complete followed by incomplete, or incomplete by itself
static mgcdata_t MsgGetCompleteData[] = 
{
    // Test case for messages with a terminating null.
    // We expect MsgGetComplete to be called 5 times. We expect 5 complete messages to be obtained.
    { "ABCDEF\0GHIJ\0KLMNO\0PQRSTUV\0WXYZ\0", 31, "N", 
        5, { "ABCDEF\0", "GHIJ\0", "KLMNO\0", "PQRSTUV\0", "WXYZ\0" }, { 7, 5, 6, 8, 5 }, { 1, 1, 1, 1, 1 } },
    // Test case for messages with a terminating null.
    // We expect MsgGetComplete to be called 1 time. We expect 1 complete message to be obtained.
    { "ABCDEFGHIJKLMNOPQRSTUVWXYZ\0", 27, "N", 
        1, { "ABCDEFGHIJKLMNOPQRSTUVWXYZ\0" }, { 27 }, { 1 } },
    // Test case for messages without a terminating null.
    // We expect MsgGetComplete to be called 1 time. We expect 1 incomplete message to be obtained.
    { "ABCDEFGHIJKLMNOPQRSTUVWXYZ", 26, "N", 
        1, { "ABCDEFGHIJKLMNOPQRSTUVWXYZ" }, { 26 }, { 0 } },
    // Test case for messages with a terminating null.
    // We expect MsgGetComplete to be called 2 times. We expect 1 complete and 1 incomplete message to be obtained.
    { "ABCDEFGHIJKLMNOPQRSTUVWXYZ\0A", 28, "N", 
        2, { "ABCDEFGHIJKLMNOPQRSTUVWXYZ\0", "A" }, { 27, 1 }, { 1, 0 } },

    // Test case for messages with a length in the message itself.
    // We expect MsgGetComplete to be called 3 times. We expect 3 complete messages to be obtained.
    // Note the hex values for the length field (2 bytes for short). The length does not include the length field itself.
    // 0x0006 is length 6 for "ABCDEF", 0x0009 is length 9 for "GHIJKLMNO" and 0x000B is length 11 for "PQRSTUVWXYZ". 
    { "\x00" "\x06" "ABCDEF" "\x00" "\x09" "GHIJKLMNO" "\x00" "\x0B" "PQRSTUVWXYZ", 32, "M", 
        3, { "\x00" "\x06" "ABCDEF", "\x00" "\x09" "GHIJKLMNO", "\x00" "\x0B" "PQRSTUVWXYZ" },  { 8, 11, 13 }, { 1, 1, 1 } },
    // Test case for messages with a length in the message itself.
    // We expect MsgGetComplete to be called 1 time. We expect 1 complete message to be obtained.
    { "\x00" "\x1A" "ABCDEFGHIJKLMNOPQRSTUVWXYZ", 28, "M", 
        1, { "\x00" "\x1A" "ABCDEFGHIJKLMNOPQRSTUVWXYZ" }, { 28 }, { 1 } },
    // Test case for messages with a length in the message itself.
    // We expect MsgGetComplete to be called 1 time. We expect 1 incomplete message to be obtained.
    { "\x00" "\x1A" "ABCDEFGHIJKLMNOPQRSTUVWXY", 27, "M", 
        1, { "\x00" "\x1A" "ABCDEFGHIJKLMNOPQRSTUVWXY" }, { 27 }, { 0 } },
    // Test case for messages with a length in the message itself.
    // We expect MsgGetComplete to be called 2 times. We expect 1 complete and 1 incomplete message to be obtained.
    { "\x00" "\x1A" "ABCDEFGHIJKLMNOPQRSTUVWXYZ" "\x00", 29, "M", 
        2, { "\x00" "\x1A" "ABCDEFGHIJKLMNOPQRSTUVWXYZ", "\x00" }, { 28, 1 }, { 1, 0 } },

    // Test case for messages with a length in the message itself.
    // We expect MsgGetComplete to be called 3 times. We expect 3 complete messages to be obtained.
    // Note the hex values for the length field (2 bytes for short). The length includes the length field.
    // 0x0008 is length 8 for "ABCDEF" + 2 for length field, 0x000B is length 11 for "GHIJKLMNO" + 2 for length field
    // and 0x000D is length 13 for "PQRSTUVWXYZ" + 2 for length field. 
    { "\x00" "\x08" "ABCDEF" "\x00" "\x0B" "GHIJKLMNO" "\x00" "\x0D" "PQRSTUVWXYZ", 32, "T", 
        3, { "\x00" "\x08" "ABCDEF", "\x00" "\x0B" "GHIJKLMNO", "\x00" "\x0D" "PQRSTUVWXYZ" },  { 8, 11, 13 }, { 1, 1, 1} },
    // Test case for messages with a length in the message itself.
    // We expect MsgGetComplete to be called 1 time. We expect 1 complete message to be obtained.
    { "\x00" "\x1C" "ABCDEFGHIJKLMNOPQRSTUVWXYZ", 28, "T", 
        1, { "\x00" "\x1C" "ABCDEFGHIJKLMNOPQRSTUVWXYZ" }, { 28 }, { 1 }},
    // Test case for messages with a length in the message itself.
    // We expect MsgGetComplete to be called 1 time. We expect 1 incomplete message to be obtained.
    { "\x00" "\x1C" "ABCDEFGHIJKLMNOPQRSTUVWXY", 27, "T", 
        1, { "\x00" "\x1C" "ABCDEFGHIJKLMNOPQRSTUVWXY" }, { 27 }, { 0 }},
    // Test case for messages with a length in the message itself.
    // We expect MsgGetComplete to be called 2 times. We expect 1 complete and 1 incomplete message to be obtained.
    { "\x00" "\x1C" "ABCDEFGHIJKLMNOPQRSTUVWXYZ" "\x00", 29, "T", 
        2, { "\x00" "\x1C" "ABCDEFGHIJKLMNOPQRSTUVWXYZ", "\x00" }, { 28, 1 }, { 1, 0 }},
};

static std::map<std::string, mgcdata_t> MsgGetCompleteD
{
    { "WithIndNMultiMsg", MsgGetCompleteData[0] },
    { "WithIndN", MsgGetCompleteData[1] },
    { "IncompleteWithIndN", MsgGetCompleteData[2] },
    { "BothWithIndN", MsgGetCompleteData[3] },

    { "WithIndMMultiMsg", MsgGetCompleteData[4] },
    { "WithIndM", MsgGetCompleteData[5] },
    { "IncompleteWithIndM", MsgGetCompleteData[6] },
    { "BothWithIndM", MsgGetCompleteData[7] },

    { "WithIndTMultiMsg", MsgGetCompleteData[8] },
    { "WithIndT", MsgGetCompleteData[9] },
    { "IncompleteWithIndT", MsgGetCompleteData[10] },
    { "BothWithIndT", MsgGetCompleteData[11] },
};

class MsgGetCompleteF : public testing::TestWithParam<std::pair<const std::string, mgcdata_t>>
{
};

INSTANTIATE_TEST_SUITE_P(msglenrtns, MsgGetCompleteF, testing::ValuesIn(MsgGetCompleteD),
[] (const testing::TestParamInfo<MsgGetCompleteF::ParamType>& info)
{
    return info.param.first;
});

TEST_P(MsgGetCompleteF, IsOk)
{
    // Arrange
    char * msg = NULL;
    char * prevMsg = NULL;
    int msgLen = 0;
    int prevMsgLen = 0;
    bool_t msgComplete = 0;

    constr_t mapKey = strchr(testing::UnitTest::GetInstance()->current_test_info()->name(), '/') + 1;

    mgcdata_t mgcdata = GetParam().second;

    prevMsg = (char *) MemAlloc("caller", mgcdata.bytesRead);
    prevMsgLen = mgcdata.bytesRead;
    memcpy(prevMsg, mgcdata.readBuf, mgcdata.bytesRead);

    // Act
    int expPrevMsgLen = prevMsgLen;
    int offset = 0;
    for (int i = 0; i < mgcdata.expMaxMsg; i++)
    {
        MsgGetComplete(mapKey, mgcdata.msglenInd, &msg, &msgLen, &prevMsg, &prevMsgLen, &msgComplete);

        EXPECT_EQ(mgcdata.expMsgComplete[i], msgComplete);

        if (msgComplete)
        {
            EXPECT_EQ(mgcdata.expMsgLen[i], msgLen);
            EXPECT_EQ(0, memcmp(mgcdata.expMsg[i], msg, msgLen));
            expPrevMsgLen -= msgLen;
            offset += msgLen;
            EXPECT_EQ(expPrevMsgLen, prevMsgLen);
            EXPECT_EQ(0, memcmp(mgcdata.readBuf + offset, prevMsg, prevMsgLen));
        }
        else
        {
            EXPECT_EQ(0, msgLen);
            EXPECT_EQ(nullptr, msg);
            EXPECT_EQ(mgcdata.expMsgLen[i], prevMsgLen);
            EXPECT_EQ(0, memcmp(mgcdata.expMsg[i], prevMsg, prevMsgLen));
        }

        MemFree(mapKey, &msg);
        msgLen = 0;
    }    

    EXPECT_EQ(expPrevMsgLen, prevMsgLen);
    MemFree(mapKey, &prevMsg);
}

TEST_P(MsgGetCompleteF, IsOkIsMsgComplete)
{
    // Arrange
    char * prevMsg = NULL;
    int prevMsgLen = 0;

    constr_t mapKey = strchr(testing::UnitTest::GetInstance()->current_test_info()->name(), '/') + 1;

    mgcdata_t mgcdata = GetParam().second;

    prevMsg = (char *) MemAlloc("caller", mgcdata.bytesRead);
    prevMsgLen = mgcdata.bytesRead;
    memcpy(prevMsg, mgcdata.readBuf, mgcdata.bytesRead);

    // Act
    for (int i = 0; i < mgcdata.expMaxMsg; i++)
    {
        int actLenInMsg;

        bool_t actMsgComplete = IsMsgComplete(mgcdata.msglenInd, prevMsg, prevMsgLen, &actLenInMsg);

        ASSERT_EQ(mgcdata.expMsgComplete[i], actMsgComplete);

        if (actMsgComplete)
        {
            ASSERT_EQ(mgcdata.expMsgLen[i], actLenInMsg);
            memcpy(prevMsg, prevMsg + actLenInMsg, prevMsgLen - actLenInMsg);
            prevMsgLen -= actLenInMsg;
        }
    }    

    MemFree(mapKey, &prevMsg);
}
