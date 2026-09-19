# Based on cmake --ver, we have 3.28.3
cmake_minimum_required(VERSION 3.25)

project(GoogleTestC VERSION 0.1.0 LANGUAGES C CXX)

set (MY_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/master)

#Set directories. The dot representes the root application folder.
set(INCLUDES_FOR_CIPHER ${MY_SOURCE_DIR}/include/cipher)
set(INCLUDES_FOR_DBIO ${MY_SOURCE_DIR}/include/dbio)
set(INCLUDES_FOR_DEFS ${MY_SOURCE_DIR}/include/defs)
set(INCLUDES_FOR_LIBRARY ${MY_SOURCE_DIR}/include/library)
set(INCLUDES_FOR_PROGRAMS ${MY_SOURCE_DIR}/include/programs)
set(INCLUDES_FOR_RECORD ${MY_SOURCE_DIR}/include/record)
set(INCLUDES_FOR_UTILS ${MY_SOURCE_DIR}/include/utils)

set(SRC_FOR_APP ${MY_SOURCE_DIR}/src/app)
set(SRC_FOR_CIPHER ${MY_SOURCE_DIR}/src/cipher)
set(SRC_FOR_DBIO ${MY_SOURCE_DIR}/src/dbio)
set(SRC_FOR_LIBRARY ${MY_SOURCE_DIR}/src/library)
set(SRC_FOR_PROGRAMS ${MY_SOURCE_DIR}/src/programs)
set(SRC_FOR_UTILS ${MY_SOURCE_DIR}/src/utils)

#Include the directories and now your cpp files will recognize your headers
include_directories(${INCLUDES_FOR_CIPHER})
include_directories(${INCLUDES_FOR_DBIO})
include_directories(${INCLUDES_FOR_DEFS})
include_directories(${INCLUDES_FOR_LIBRARY})
include_directories(${INCLUDES_FOR_PROGRAMS})
include_directories(${INCLUDES_FOR_RECORD})
include_directories(${INCLUDES_FOR_UTILS})

include_directories(${SRC_FOR_APP})
include_directories(${SRC_FOR_CIPHER})
include_directories(${SRC_FOR_DBIO})
include_directories(${SRC_FOR_LIBRARY})
include_directories(${SRC_FOR_PROGRAMS})
include_directories(${SRC_FOR_UTILS})

include_directories(${GTEST_INCLUDE_DIR})

set (CMAKE_C_FLAGS "-Wall -g -fprofile-arcs -ftest-coverage")

set(
    SRC_FILES
    ${SRC_FOR_APP}/main.c

    # ${INCLUDES_FOR_CIPHER}/iopersoncipher.h
    # ${INCLUDES_FOR_CIPHER}/iopmttxncipher.h

    # ${INCLUDES_FOR_DBIO}/ioperson.h
    # ${INCLUDES_FOR_DBIO}/iopersonrtns.h
    # ${INCLUDES_FOR_DBIO}/iopmttxn.h
    # ${INCLUDES_FOR_DBIO}/iopmttxnrtns.h
    # ${INCLUDES_FOR_DBIO}/iorecprm.h
    # ${INCLUDES_FOR_DBIO}/iorecprmrtns.h

    # ${INCLUDES_FOR_DEFS}/errmsgdefs.h
    # ${INCLUDES_FOR_DEFS}/syssizes.h
    # ${INCLUDES_FOR_DEFS}/typedefs.h

    # ${INCLUDES_FOR_LIBRARY}/ccchecksum.h
    # ${INCLUDES_FOR_LIBRARY}/ccrtns.h
    # ${INCLUDES_FOR_LIBRARY}/errmsgrtns.h

    # ${INCLUDES_FOR_RECORD}/person.h
    # ${INCLUDES_FOR_RECORD}/pmttxn.h
    # ${INCLUDES_FOR_RECORD}/recprm.h

    # ${INCLUDES_FOR_UTILS}/strutils.h

    ${SRC_FOR_CIPHER}/iopersoncipher.c
    ${SRC_FOR_CIPHER}/iopmttxncipher.c

    ${SRC_FOR_DBIO}/ioperson.c
    ${SRC_FOR_DBIO}/iopersonrtns.c
    ${SRC_FOR_DBIO}/iopmttxn.c
    ${SRC_FOR_DBIO}/iopmttxnrtns.c
    ${SRC_FOR_DBIO}/iorecprm.c
    ${SRC_FOR_DBIO}/iorecprmrtns.c

    ${SRC_FOR_LIBRARY}/ccchecksum.c
    ${SRC_FOR_LIBRARY}/ccrtns.c
    ${SRC_FOR_LIBRARY}/errmsgrtns.c
    ${SRC_FOR_LIBRARY}/filertns.c
    ${SRC_FOR_LIBRARY}/fileutils.c
    ${SRC_FOR_LIBRARY}/lockfilertns.c
    ${SRC_FOR_LIBRARY}/pathstatus.c
    ${SRC_FOR_LIBRARY}/socketrecv.c
    ${SRC_FOR_LIBRARY}/socketsend.c
    ${SRC_FOR_LIBRARY}/syscmd.c
    ${SRC_FOR_LIBRARY}/tcprtns.c

    ${SRC_FOR_PROGRAMS}/receipt.c
    ${SRC_FOR_PROGRAMS}/receiptval.c
    ${SRC_FOR_PROGRAMS}/receiptwrite.c

    ${SRC_FOR_UTILS}/cmdrtns.c
    ${SRC_FOR_UTILS}/csvrtns.c
    ${SRC_FOR_UTILS}/cyclicstr.c
    ${SRC_FOR_UTILS}/daemonrtns.c
    ${SRC_FOR_UTILS}/datmutils.c
    ${SRC_FOR_UTILS}/fmtamt.c
    ${SRC_FOR_UTILS}/fmtdate.c
    ${SRC_FOR_UTILS}/fmtstr.c
    ${SRC_FOR_UTILS}/hashpwd.c
    ${SRC_FOR_UTILS}/llfcntl.c
    ${SRC_FOR_UTILS}/llmemrtns.c
    ${SRC_FOR_UTILS}/llpoll.c
    ${SRC_FOR_UTILS}/llsocket.c
    ${SRC_FOR_UTILS}/llstdio.c
    ${SRC_FOR_UTILS}/llunistd.c
    ${SRC_FOR_UTILS}/lnklstrtns.c
    ${SRC_FOR_UTILS}/logcmds.c
    ${SRC_FOR_UTILS}/logfilertns.c
    ${SRC_FOR_UTILS}/logpkgcmds.c
    ${SRC_FOR_UTILS}/logpkgrtns.c
    ${SRC_FOR_UTILS}/logrtns.c
    ${SRC_FOR_UTILS}/memrtns.c
    ${SRC_FOR_UTILS}/msglenrtns.c
    ${SRC_FOR_UTILS}/pthreadrtns.c
    ${SRC_FOR_UTILS}/strutils.c
    ${SRC_FOR_UTILS}/sysexit.c
    ${SRC_FOR_UTILS}/thrinfo.c
)

add_executable(
    mysys
    ${SRC_FILES}
)

find_package(Threads REQUIRED)
target_link_libraries(mysys PRIVATE Threads::Threads)
find_package(PkgConfig REQUIRED)
pkg_check_modules(XCRYPT REQUIRED IMPORTED_TARGET libxcrypt)
target_link_libraries(mysys PRIVATE PkgConfig::XCRYPT)