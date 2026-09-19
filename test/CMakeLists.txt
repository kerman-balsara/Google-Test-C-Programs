# Based on cmake --ver, we have 3.28.3
cmake_minimum_required(VERSION 3.25)

project(GoogleTestC VERSION 0.1.0 LANGUAGES C CXX)

set (MY_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../master)
set (MY_GTEST_DIR /usr/src/googletest)

enable_testing()

set(GTEST_INCLUDE_DIR ${MY_GTEST_DIR}/googletest/include)
set(GTEST_LIBRARY ${MY_GTEST_DIR}/build/lib/libgtest.so)
set(GTEST_MAIN_LIBRARY ${MY_GTEST_DIR}/build/lib/libgtest_main.so)

set(GMOCK_INCLUDE_DIR ${MY_GTEST_DIR}/googlemock/include)

find_package(GTest REQUIRED)

#Set directories. The dot representes the root application folder.
set(INCLUDES_FOR_CIPHER ${MY_SOURCE_DIR}/include/cipher)
set(INCLUDES_FOR_DBIO ${MY_SOURCE_DIR}/include/dbio)
set(INCLUDES_FOR_DEFS ${MY_SOURCE_DIR}/include/defs)
set(INCLUDES_FOR_LIBRARY ${MY_SOURCE_DIR}/include/library)
set(INCLUDES_FOR_RECORD ${MY_SOURCE_DIR}/include/record)
set(INCLUDES_FOR_UTILS ${MY_SOURCE_DIR}/include/utils)

set(SRC_FOR_CIPHER ${MY_SOURCE_DIR}/src/cipher)
set(SRC_FOR_DBIO ${MY_SOURCE_DIR}/src/dbio)
set(SRC_FOR_LIBRARY ${MY_SOURCE_DIR}/src/library)
set(SRC_FOR_UTILS ${MY_SOURCE_DIR}/src/utils)

set(MYSYS_OBJ_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../build/CMakeFiles/mysys.dir/master/src)

set(OBJ_FOR_CIPHER ${MYSYS_OBJ_DIR}/cipher)
set(OBJ_FOR_DBIO ${MYSYS_OBJ_DIR}/dbio)
set(OBJ_FOR_LIBRARY ${MYSYS_OBJ_DIR}/library)
set(OBJ_FOR_PROGRAMS ${MYSYS_OBJ_DIR}/programs)
set(OBJ_FOR_UTILS ${MYSYS_OBJ_DIR}/utils)

set(TESTS_LIB ${CMAKE_CURRENT_SOURCE_DIR})

#Include the directories and now your cpp files will recognize your headers
include_directories(${INCLUDES_FOR_CIPHER})
include_directories(${INCLUDES_FOR_DBIO})
include_directories(${INCLUDES_FOR_DEFS})
include_directories(${INCLUDES_FOR_LIBRARY})
include_directories(${INCLUDES_FOR_RECORD})
include_directories(${INCLUDES_FOR_UTILS})

include_directories(${SRC_FOR_CIPHER})
include_directories(${SRC_FOR_DBIO})
include_directories(${SRC_FOR_LIBRARY})
include_directories(${SRC_FOR_UTILS})

include_directories(${TESTS_LIB})

include_directories(${GTEST_INCLUDE_DIR})
include_directories(${GMOCK_INCLUDE_DIR})

set(
    SRC_FILES

    ${TESTS_LIB}/library/ccchecksum.cpp
    ${TESTS_LIB}/library/ccrtns.cpp
    ${TESTS_LIB}/library/errmsgrtns.cpp
    ${TESTS_LIB}/library/filertns.cpp
    ${TESTS_LIB}/library/fileutils.cpp
    ${TESTS_LIB}/library/pathstatus.cpp
    ${TESTS_LIB}/library/syscmd.cpp
    ${TESTS_LIB}/library/tcprtns.cpp

    ${TESTS_LIB}/misc/structinit.cpp

    ${TESTS_LIB}/utils/cmdrtns.cpp
    ${TESTS_LIB}/utils/csvrtns.cpp
    ${TESTS_LIB}/utils/cyclicstr.cpp
    ${TESTS_LIB}/utils/datmutils.cpp
    ${TESTS_LIB}/utils/fmtamt.cpp
    ${TESTS_LIB}/utils/fmtdate.cpp
    ${TESTS_LIB}/utils/fmtstr.cpp
    ${TESTS_LIB}/utils/hashpwd.cpp
    ${TESTS_LIB}/utils/lnklstrtns.cpp
    ${TESTS_LIB}/utils/logpkgcmds.cpp
    ${TESTS_LIB}/utils/logpkgrtns.cpp
    ${TESTS_LIB}/utils/logcmds.cpp
    ${TESTS_LIB}/utils/logrtns.cpp
    ${TESTS_LIB}/utils/memrtns.cpp
    ${TESTS_LIB}/utils/msglenrtns.cpp
    ${TESTS_LIB}/utils/strutils.cpp
    ${TESTS_LIB}/utils/sysexit.cpp
    ${TESTS_LIB}/utils/thrinfo.cpp
)

set(
    OBJ_FILES

    ${OBJ_FOR_CIPHER}/iopersoncipher.c.o
    ${OBJ_FOR_CIPHER}/iopmttxncipher.c.o

    ${OBJ_FOR_DBIO}/ioperson.c.o
    ${OBJ_FOR_DBIO}/iopersonrtns.c.o
    ${OBJ_FOR_DBIO}/iopmttxn.c.o
    ${OBJ_FOR_DBIO}/iopmttxnrtns.c.o
    ${OBJ_FOR_DBIO}/iorecprm.c.o
    ${OBJ_FOR_DBIO}/iorecprmrtns.c.o

    ${OBJ_FOR_LIBRARY}/ccchecksum.c.o
    ${OBJ_FOR_LIBRARY}/ccrtns.c.o
    ${OBJ_FOR_LIBRARY}/errmsgrtns.c.o
    ${OBJ_FOR_LIBRARY}/filertns.c.o
    ${OBJ_FOR_LIBRARY}/fileutils.c.o
    ${OBJ_FOR_LIBRARY}/pathstatus.c.o
    ${OBJ_FOR_LIBRARY}/socketrecv.c.o
    ${OBJ_FOR_LIBRARY}/socketsend.c.o
    ${OBJ_FOR_LIBRARY}/syscmd.c.o
    ${OBJ_FOR_LIBRARY}/tcprtns.c.o

    ${OBJ_FOR_UTILS}/cmdrtns.c.o
    ${OBJ_FOR_UTILS}/csvrtns.c.o
    ${OBJ_FOR_UTILS}/cyclicstr.c.o
    ${OBJ_FOR_UTILS}/datmutils.c.o
    ${OBJ_FOR_UTILS}/fmtamt.c.o
    ${OBJ_FOR_UTILS}/fmtdate.c.o
    ${OBJ_FOR_UTILS}/fmtstr.c.o
    ${OBJ_FOR_UTILS}/hashpwd.c.o
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

find_package(PkgConfig REQUIRED)
pkg_check_modules(XCRYPT REQUIRED IMPORTED_TARGET libxcrypt)

target_link_libraries(
mysystest
${GTEST_LIBRARY}
${GTEST_MAIN_LIBRARY}
gcov
PkgConfig::XCRYPT
)

include(CTest)
include(GoogleTest)
gtest_discover_tests(mysystest)
