#include <gtest/gtest.h>
#include <gmock/gmock.h>

// #include <loglvldefs.h>
#include <person.h>

#include <lnklstrtns.h>
// #include <logrtns.h>
#include <memrtns.h>

typedef struct person_s
{
    struct person_s *next;
    person_r person;
} person_l;

static person_l * head = NULL;
static person_l * tail = NULL;

#define totalElems 5

class lnklstrtnsCommonF
{
    protected:
    static void SetUpTestSuite()
    {
        // LogLvlEnable(LOGLVL_TRACE);
    }

    static void TearDownTestSuite()
    {
        // LogLvlDisable(LOGLVL_TRACE);
    }

    static void SetUp()
    {
        for (int i = 0; i < totalElems; i++)
        {
            person_l * elem = (person_l *) MemAlloc("lnklstrtnsF", sizeof(person_l));
            elem->person.id = i + 1;
            LnkLstAppend(elem, &head, &tail);
        }
        ASSERT_NE(nullptr, head);
        EXPECT_EQ(totalElems, LnkLstCount(head));
        // fprintf(stderr, "Head %p, Ptr %p\n", &head, head);
        // fprintf(stderr, "Tail %p, Ptr %p\n", &tail, tail);
    }

    static void TearDown()
    {
        LnkLstFree("lnklstrtnsF", &head);
    }
};

static std::map<std::string, std::array<int, totalElems>> LnkLstRemoveD
{
    {"LnkLstRemoveFwd", { 0, 0, 0, 0, 0 } },
    {"LnkLstRemoveRev", { 4, 3, 2, 1, 0 } },
    {"LnkLstRemoveRnd", { 2, 2, 1, 1, 0 } },
};

class LnkLstRemoveF : public lnklstrtnsCommonF, public testing::TestWithParam<std::pair<const std::string, std::array<int, 5>>>
{
    protected:
    static void SetUpTestSuite()
    {
        lnklstrtnsCommonF::SetUpTestSuite();
    }
    static void TearDownTestSuite()
    {
        lnklstrtnsCommonF::TearDownTestSuite();
    }

    void SetUp() override
    {
        lnklstrtnsCommonF::SetUp();
    }

    void TearDown() override
    {
        lnklstrtnsCommonF::TearDown();
    }
};

INSTANTIATE_TEST_SUITE_P(lnklstrtns, LnkLstRemoveF, testing::ValuesIn(LnkLstRemoveD),
[] (const testing::TestParamInfo<LnkLstRemoveF::ParamType>& info)
{
    return info.param.first;
});

TEST_P(LnkLstRemoveF, IsOk)
{
    // Arrange
    std::array<int, totalElems> elemIdx = GetParam().second;

    constr_t mapKey = strchr(testing::UnitTest::GetInstance()->current_test_info()->name(), '/') + 1;

    int expElems = totalElems;
    // Act & Assert
    for (int i = 0; i < totalElems; i++)
    {
        person_l * curr = head;

        for (int j = 0; j < elemIdx[i] ; curr = curr->next, j++);

        LnkLstRemove(mapKey, &head, &curr, &tail);

        --expElems;

        if (expElems == 0)
        {
            ASSERT_EQ(nullptr, head) << mapKey << " failed";
            ASSERT_EQ(nullptr, tail) << mapKey << " failed";
        }
        else
        {
            ASSERT_NE(nullptr, head) << mapKey << " failed";
            ASSERT_NE(nullptr, tail) << mapKey << " failed";
        }

        EXPECT_EQ(expElems, LnkLstCount(head)) << mapKey << " failed";
    }
}

class LnkLstFreeF : public lnklstrtnsCommonF, public ::testing::Test
{
    protected:
    static void SetUpTestSuite()
    {
        lnklstrtnsCommonF::SetUpTestSuite();
    }
    static void TearDownTestSuite()
    {
        lnklstrtnsCommonF::TearDownTestSuite();
    }
    void SetUp() override
    {
        lnklstrtnsCommonF::SetUp();
    }
    void TearDown() override
    {
        lnklstrtnsCommonF::TearDown();
    }
};

TEST_F(LnkLstFreeF, IsOk)
{
    // Act
    LnkLstFree("LnkLstFree", &head);
    ASSERT_EQ(nullptr, head);
    EXPECT_EQ(0, LnkLstCount(head));    

    // Calling LnkLstFree() with a null pointer should have no effect. 
    LnkLstFree("LnkLstFree", &head);
    ASSERT_EQ(nullptr, head);
    EXPECT_EQ(0, LnkLstCount(head));    
}

TEST(lnklstrtns, IsOk)
{
    head = NULL;
    tail = NULL;
    
    person_l * elem1 = (person_l *) MemAlloc("LnkLstAll", sizeof(person_l));
    elem1->person.id = 100;
    LnkLstAppend(elem1, &head, &tail);
    person_l * elem2 = (person_l *) MemAlloc("LnkLstAll", sizeof(person_l));
    elem2->person.id = 200;
    LnkLstAppend(elem2, &head, &tail);

    EXPECT_EQ(2, LnkLstCount(head));

    EXPECT_EQ(elem1, head);
    EXPECT_EQ(100, head->person.id);

    EXPECT_EQ(elem2, tail);
    EXPECT_EQ(200, tail->person.id);

    EXPECT_EQ(nullptr, LnkLstPrev(head, elem1));
    EXPECT_EQ(head, LnkLstPrev(head, elem2));

    EXPECT_EQ(tail, LnkLstTail(head));

    LnkLstFree("LnkLstAll", &head);
    EXPECT_EQ(0, LnkLstCount(head));
    EXPECT_EQ(nullptr, head);
    EXPECT_EQ(nullptr, LnkLstTail(head));
    EXPECT_EQ(nullptr, LnkLstPrev(head, NULL));
}
