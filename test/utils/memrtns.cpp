#include <gtest/gtest.h>
#include <gmock/gmock.h>

using ::testing::StartsWith;

#include <person.h>

#include <loglvldefs.h>
#include <logrtns.h>
#include <memrtns.h>

TEST(MemAllocAndFree, IsOk)
{
    // Arrange
    person_r * personElem = (person_r *) MemAlloc("MemAllocFree", sizeof(person_r));
    ASSERT_NE(nullptr, personElem);

    MemFree("MemAllocFree", &personElem);
    ASSERT_EQ(nullptr, personElem);
}

TEST(MemAllocReallocAndFree, IsOk)
{
    // Arrange
    char * charPtr = NULL;
    charPtr = (char *) MemAlloc("MemRellocFree", 4);
    ASSERT_NE(nullptr, charPtr);
    memcpy(charPtr, "AAAA", 4);
    MemRealloc("MemRellocFree", &charPtr, 10);
    ASSERT_NE(nullptr, charPtr);
    EXPECT_THAT(charPtr, StartsWith("AAAA"));

    MemFree("MemRellocFree", &charPtr);
    ASSERT_EQ(nullptr, charPtr);
}