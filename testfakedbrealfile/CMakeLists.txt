# Based on cmake --ver, we have 3.28.3
cmake_minimum_required(VERSION 3.25)

project(GoogleTestC VERSION 0.1.0 LANGUAGES C CXX)

set (MY_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../master)
set (MY_GTEST_DIR /usr/src/googletest)

enable_testing()

set(GTEST_INCLUDE_DIR ${MY_GTEST_DIR}/googletest/include)
set(GMOCK_INCLUDE_DIR ${MY_GTEST_DIR}/googlemock/include)
set(GTEST_LIBRARY ${MY_GTEST_DIR}/build/lib/libgtest.so)
set(GTEST_MAIN_LIBRARY ${MY_GTEST_DIR}/build/lib/libgtest_main.so)
set(GMOCK_LIBRARY ${MY_GTEST_DIR}/build/lib/libgmock.so)
set(GMOCK_MAIN_LIBRARY ${MY_GTEST_DIR}/build/lib/libgmock_main.so)

find_package(GTest REQUIRED)

#Set directories. The dot representes the root application folder.
set(INCLUDES_FOR_CIPHER ${MY_SOURCE_DIR}/include/cipher)
set(INCLUDES_FOR_DBIO ${MY_SOURCE_DIR}/include/dbio)
set(INCLUDES_FOR_DEFS ${MY_SOURCE_DIR}/include/defs)
set(INCLUDES_FOR_LIBRARY ${MY_SOURCE_DIR}/include/library)
set(INCLUDES_FOR_PROGRAMS ${MY_SOURCE_DIR}/include/programs)
set(INCLUDES_FOR_RECORD ${MY_SOURCE_DIR}/include/record)
set(INCLUDES_FOR_UTILS ${MY_SOURCE_DIR}/include/utils)

set(SRC_FOR_CIPHER ${MY_SOURCE_DIR}/src/cipher)
set(SRC_FOR_DBIO ${MY_SOURCE_DIR}/src/dbio)
set(SRC_FOR_LIBRARY ${MY_SOURCE_DIR}/src/library)
set(SRC_FOR_PROGRAMS ${MY_SOURCE_DIR}/src/programs)
set(SRC_FOR_UTILS ${MY_SOURCE_DIR}/src/utils)

set(MYSYS_OBJ_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../build/CMakeFiles/mysys.dir/master/src)

set(OBJ_FOR_CIPHER ${MYSYS_OBJ_DIR}/cipher)
set(OBJ_FOR_DBIO ${MYSYS_OBJ_DIR}/dbio)
set(OBJ_FOR_LIBRARY ${MYSYS_OBJ_DIR}/library)
set(OBJ_FOR_PROGRAMS ${MYSYS_OBJ_DIR}/programs)
set(OBJ_FOR_UTILS ${MYSYS_OBJ_DIR}/utils)

set(TESTS_LIB ${CMAKE_CURRENT_SOURCE_DIR})
set(TEST_HELPER_LIB ${CMAKE_CURRENT_SOURCE_DIR}/../testhelper)

#Include the directories and now your cpp files will recognize your headers
include_directories(${INCLUDES_FOR_CIPHER})
include_directories(${INCLUDES_FOR_DBIO})
include_directories(${INCLUDES_FOR_DEFS})
include_directories(${INCLUDES_FOR_LIBRARY})
include_directories(${INCLUDES_FOR_PROGRAMS})
include_directories(${INCLUDES_FOR_RECORD})
include_directories(${INCLUDES_FOR_UTILS})

include_directories(${SRC_FOR_DBIO})
include_directories(${SRC_FOR_CIPHER})
include_directories(${SRC_FOR_LIBRARY})
include_directories(${SRC_FOR_PROGRAMS})

include_directories(${TESTS_LIB})
include_directories(${TEST_HELPER_LIB})

include_directories(${GTEST_INCLUDE_DIR})
include_directories(${GMOCK_INCLUDE_DIR})

set(
    SRC_FILES

    ${TEST_HELPER_LIB}/persondata.c
    ${TEST_HELPER_LIB}/pmttxndata.c
    ${TEST_HELPER_LIB}/recprmdata.c

    ${TESTS_LIB}/misc/fakedfns.cpp

    ${TESTS_LIB}/programs/receipt.cpp
)

# Files with functions that have been faked in fakedfns.cpp must not be included here
# iopersonrtns.c.o
# iopersoncipher.c.o
# iopmttxnrtns.c.o
# iopmttxncipher.c.o
# iorecprmrtns.c.o
set(
    OBJ_FILES

    ${OBJ_FOR_DBIO}/ioperson.c.o
    ${OBJ_FOR_DBIO}/iopmttxn.c.o
    ${OBJ_FOR_DBIO}/iorecprm.c.o

    ${OBJ_FOR_LIBRARY}/errmsgrtns.c.o
    ${OBJ_FOR_LIBRARY}/filertns.c.o
    ${OBJ_FOR_LIBRARY}/fileutils.c.o
    ${OBJ_FOR_LIBRARY}/pathstatus.c.o
    ${OBJ_FOR_LIBRARY}/socketrecv.c.o
    ${OBJ_FOR_LIBRARY}/socketsend.c.o
    ${OBJ_FOR_LIBRARY}/syscmd.c.o
    ${OBJ_FOR_LIBRARY}/tcprtns.c.o

    ${OBJ_FOR_PROGRAMS}/receipt.c.o
    ${OBJ_FOR_PROGRAMS}/receiptval.c.o
    ${OBJ_FOR_PROGRAMS}/receiptwrite.c.o

    ${OBJ_FOR_UTILS}/cmdrtns.c.o
    ${OBJ_FOR_UTILS}/csvrtns.c.o
    ${OBJ_FOR_UTILS}/cyclicstr.c.o
    ${OBJ_FOR_UTILS}/datmutils.c.o
    ${OBJ_FOR_UTILS}/fmtamt.c.o
    ${OBJ_FOR_UTILS}/fmtdate.c.o
    ${OBJ_FOR_UTILS}/fmtstr.c.o
    ${OBJ_FOR_UTILS}/llfcntl.c.o
    ${OBJ_FOR_UTILS}/llmemrtns.c.o
    ${OBJ_FOR_UTILS}/llpoll.c.o
    ${OBJ_FOR_UTILS}/llsocket.c.o
    ${OBJ_FOR_UTILS}/llstdio.c.o
    ${OBJ_FOR_UTILS}/llunistd.c.o
    ${OBJ_FOR_UTILS}/lnklstrtns.c.o
    ${OBJ_FOR_UTILS}/logcmds.c.o
    ${OBJ_FOR_UTILS}/logpkgcmds.c.o
    ${OBJ_FOR_UTILS}/logpkgrtns.c.o
    ${OBJ_FOR_UTILS}/logrtns.c.o
    ${OBJ_FOR_UTILS}/memrtns.c.o
    ${OBJ_FOR_UTILS}/msglenrtns.c.o
    ${OBJ_FOR_UTILS}/strutils.c.o
    ${OBJ_FOR_UTILS}/sysexit.c.o
    ${OBJ_FOR_UTILS}/thrinfo.c.o
)

set(CMAKE_C_FLAGS "-Wall -g -fprofile-arcs -ftest-coverage")
set(CMAKE_CXX_FLAGS "-Wall -g")

add_executable(
    mysystest
    ${SRC_FILES}
    ${OBJ_FILES}
)

target_link_libraries(
mysystest
${GTEST_LIBRARY}
${GTEST_MAIN_LIBRARY}
gcov
)

include(GoogleTest)
gtest_discover_tests(mysystest)
