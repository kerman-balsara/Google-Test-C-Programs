Project environment:
OS - Ubuntu 24.04
Programming language - C
C tools - gcc g++ gdb
Editor - vs code
Build - cmake
Test - google test
Extensions - cmake, cmake tools, c/c++ intellisense etc, c/c++ extension pack, c/c++ themes
Other - gcov

GoogleTestC:
Contains the project
- Read from database (none exists)
- Create email file
Built using cleansysbuild.sh or incrsysbuild.sh and uses its own CMakeLists.txt
Executable in build (mysys)
cleanbuildall.sh or incrbuildall.sh builds all the executables
runalltests.sh runs all the tests

GoogleTestC/master:
Contains the source and include files
Built using cleantestbuild.sh or incrtestbuild.sh and uses its own CMakeLists.txt
Executable in build (mysys)

GoogleTestC/test/...:
Contains tests that do not require the database or file io
Built using cleantestbuild.sh or incrtestbuild.sh and uses its own CMakeLists.txt
Executable in build (mysystest)
"build/mysystest --gtest_brief=1" runs the tests

GoogleTestC/testfaked/...:
Contains tests that are based on a faked database and faked file io
Built using cleantestbuild.sh or incrtestbuild.sh and uses its own CMakeLists.txt
Executable in build (mysystest)
"build/mysystest --gtest_brief=1" runs the tests

GoogleTestC/testfakedbrealfile/...:
Contains tests that are based on a faked database and real file io
Built using cleantestbuild.sh or incrtestbuild.sh and uses its own CMakeLists.txt
Executable in build (mysystest)
runtests runs the tests (also deletes files from email and archive folders)

GoogleTestC/testmock/...:
Contains tests that use mocks
Once I ran the tests using a debugger, I realised that I could use
fakes more effectively than mocks. The mocks are mainly used for testing error handling.
Built using cleantestbuild.sh or incrtestbuild.sh and uses its own CMakeLists.txt
Executable in build (mysystest)
"build/mysystest --gtest_brief=1" runs the tests

GoogleTestC/testfolder
Contains files for testing

GoogleTestC/testhelper
Contains files to return fake database records

GoogleTestC/mergegcov
Contains files to merge two gcov files and show a consolidated result

To get coverage, in /GoogleTestC, run:
./cleanbuildall.sh or ./incrbuildall.sh
./runalltests.sh
./gcoverage.sh

Check gcov.txt for test coverage.
===============================================================

After running gcoverage.sh, in a few cases (due to our testing setup), we can get multiple gcov files.
For e.g., pathstatus.c.gcov files are generated in the following folders:
* GoogleTestC/build/CMakeFiles/mysys.dir/master/src/library
* GoogleTestC/testmock/build/CMakeFiles/mysystest.dir/library

The first folder accumulates the results of testing pathstatus.c from the test, testfake, and testfakedbrealfile
folders. The second folder accumulates the results of testing pathstatus.c from the testmock folder via 
wrappathstatus.c. The tests exercise different aspects of the program pathstatus.c. [Note: wrappathstatus.c
just includes pathstatus.c. After the build, in the build folder, you will notice .gcno and .gcda files for
wrappathstatus.c, not for pathstatus.c. However, if we run gcov on the wrapstatus.c.gcda file, the coverage is
generated in pathstatus.c.gcov].

If we look at the individual gcov files in isolation, we can see that some lines of pathstatus.c have not been
executed (look for "#####") or have been partially executed (look for "n*") before the first colon (:). But if
we consider a merged file, we can see that all lines of pathstatus.c have been executed.

Source for gcov.c is at
https://raw.githubusercontent.com/gcc-mirror/gcc/releases/gcc-7.5.0/gcc/gcov.c

Running of mergegcov and moving the merged files to /GoogleTestC/build/CMakeFiles/mysys.dir/master/src/... is
now part of gcoverage.sh.

===============================================================

Test files:
Suffixes used within test files:
D - Data
F - Fixture
A - Assert

We use a map so that the key gives a better indication of the test that failed.
i.e. if the test at index 0 fails, we should see the key instead of 0.
See INSTANTIATE_TEST_SUITE_P 4th parameter which is a lamda function (nameGenerator) which generates the name.

Each entry in the map is of type std::pair.
===============================================================

Death Tests:
In case the called function uses assert() instead of exit(withCode), test with EXPECT_DEATH:
TEST(cyclicstrDeathTest, CyclicStrAddDeath)
{
   // Assert
   EXPECT_DEATH(CyclicStrAdd("..."), "");
}
