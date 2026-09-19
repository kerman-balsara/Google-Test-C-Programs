#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <pthread.h>

#include <typedefs.h>

#include <thrinfo.h>

typedef struct
{
    pthread_t id;
    constr_t name;
} thrdtls_t;

static thrdtls_t ThrDtls[] = 
{ 
    { 1000, "Timer" },
    { 2000, "CmdInput" },
    { 3000, "CmdOutput" },
};

class ThrInfoF : public ::testing::Test
{
    void SetUp() override
    {
        int count = sizeof(ThrDtls) / sizeof(thrdtls_t);
        for (int i = 0; i < count; i++)
        {
            ThrInfoAdd("ThrInfoF", ThrDtls[i].id, ThrDtls[i].name);
        }
    }
    void TearDown() override
    {
        ThrInfoFree("ThrInfoF");
    }
};

TEST_F(ThrInfoF, IsOkThrInfoAdd)
{
    // Arrange
    pthread_t id = 4000;
    constr_t name = "Info";

    // Act
    ThrInfoAdd("ThrInfoAdd", id, name);

    // Assert
    EXPECT_STREQ(name, ThrInfoFind(id));
}

TEST_F(ThrInfoF, IsOkThrInfoRemove)
{
    // Arrange
    pthread_t id = 2000;

    // Act
    ThrInfoRemove("ThrInfoRemove", id);

    // Assert (If thread id is not registered, default string "Main" is returned)
    EXPECT_STREQ("Main", ThrInfoFind(id));
}

TEST_F(ThrInfoF, IsOkThrInfoFind)
{
    // Arrange
    pthread_t id = ThrDtls[2].id;
    constr_t name = ThrDtls[2].name;

    // Act & Assert
    EXPECT_STREQ(name, ThrInfoFind(id));
}