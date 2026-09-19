
cd /home/kerman/projects/GoogleTestC/build/CMakeFiles/mysys.dir/master/src/cipher
gcov *.gcda
cd /home/kerman/projects/GoogleTestC/build/CMakeFiles/mysys.dir/master/src/dbio
gcov *.gcda
cd /home/kerman/projects/GoogleTestC/build/CMakeFiles/mysys.dir/master/src/library
gcov *.gcda
cd /home/kerman/projects/GoogleTestC/build/CMakeFiles/mysys.dir/master/src/programs
gcov *.gcda
cd /home/kerman/projects/GoogleTestC/build/CMakeFiles/mysys.dir/master/src/utils
gcov *.gcda

cd /home/kerman/projects/GoogleTestC/testmock/build/CMakeFiles/mysystest.dir/dbio
gcov *.gcda
cd /home/kerman/projects/GoogleTestC/testmock/build/CMakeFiles/mysystest.dir/library
gcov *.gcda
cd /home/kerman/projects/GoogleTestC/testmock/build/CMakeFiles/mysystest.dir/utils
gcov *.gcda

cd /home/kerman/projects/GoogleTestC/testfake/build/CMakeFiles/mysystest.dir/programs
gcov *.gcda

cd /home/kerman/projects/GoogleTestC

/home/kerman/projects/GoogleTestC/mergegcov/build/mergegcov /home/kerman/projects/GoogleTestC/testmock/build/CMakeFiles/mysystest.dir/dbio/ioperson.c.gcov /home/kerman/projects/GoogleTestC/build/CMakeFiles/mysys.dir/master/src/dbio/ioperson.c.gcov /home/kerman/projects/GoogleTestC/mergegcov/ioperson.c.gcov
mv -f /home/kerman/projects/GoogleTestC/mergegcov/ioperson.c.gcov /home/kerman/projects/GoogleTestC/build/CMakeFiles/mysys.dir/master/src/dbio

/home/kerman/projects/GoogleTestC/mergegcov/build/mergegcov /home/kerman/projects/GoogleTestC/testmock/build/CMakeFiles/mysystest.dir/library/filertns.c.gcov /home/kerman/projects/GoogleTestC/build/CMakeFiles/mysys.dir/master/src/library/filertns.c.gcov /home/kerman/projects/GoogleTestC/mergegcov/filertns.c.gcov
mv -f /home/kerman/projects/GoogleTestC/mergegcov/filertns.c.gcov /home/kerman/projects/GoogleTestC/build/CMakeFiles/mysys.dir/master/src/library

/home/kerman/projects/GoogleTestC/mergegcov/build/mergegcov /home/kerman/projects/GoogleTestC/testmock/build/CMakeFiles/mysystest.dir/library/pathstatus.c.gcov /home/kerman/projects/GoogleTestC/build/CMakeFiles/mysys.dir/master/src/library/pathstatus.c.gcov /home/kerman/projects/GoogleTestC/mergegcov/pathstatus.c.gcov
mv -f /home/kerman/projects/GoogleTestC/mergegcov/pathstatus.c.gcov /home/kerman/projects/GoogleTestC/build/CMakeFiles/mysys.dir/master/src/library

/home/kerman/projects/GoogleTestC/mergegcov/build/mergegcov /home/kerman/projects/GoogleTestC/testmock/build/CMakeFiles/mysystest.dir/library/socketrecv.c.gcov /home/kerman/projects/GoogleTestC/build/CMakeFiles/mysys.dir/master/src/library/socketrecv.c.gcov /home/kerman/projects/GoogleTestC/mergegcov/socketrecv.c.gcov
mv -f /home/kerman/projects/GoogleTestC/mergegcov/socketrecv.c.gcov /home/kerman/projects/GoogleTestC/build/CMakeFiles/mysys.dir/master/src/library

/home/kerman/projects/GoogleTestC/mergegcov/build/mergegcov /home/kerman/projects/GoogleTestC/testmock/build/CMakeFiles/mysystest.dir/library/socketsend.c.gcov /home/kerman/projects/GoogleTestC/build/CMakeFiles/mysys.dir/master/src/library/socketsend.c.gcov /home/kerman/projects/GoogleTestC/mergegcov/socketsend.c.gcov
mv -f /home/kerman/projects/GoogleTestC/mergegcov/socketsend.c.gcov /home/kerman/projects/GoogleTestC/build/CMakeFiles/mysys.dir/master/src/library

/home/kerman/projects/GoogleTestC/mergegcov/build/mergegcov /home/kerman/projects/GoogleTestC/testmock/build/CMakeFiles/mysystest.dir/library/syscmd.c.gcov /home/kerman/projects/GoogleTestC/build/CMakeFiles/mysys.dir/master/src/library/syscmd.c.gcov /home/kerman/projects/GoogleTestC/mergegcov/syscmd.c.gcov
mv -f /home/kerman/projects/GoogleTestC/mergegcov/syscmd.c.gcov /home/kerman/projects/GoogleTestC/build/CMakeFiles/mysys.dir/master/src/library

/home/kerman/projects/GoogleTestC/mergegcov/build/mergegcov /home/kerman/projects/GoogleTestC/testmock/build/CMakeFiles/mysystest.dir/library/tcprtns.c.gcov /home/kerman/projects/GoogleTestC/build/CMakeFiles/mysys.dir/master/src/library/tcprtns.c.gcov /home/kerman/projects/GoogleTestC/mergegcov/tcprtns.c.gcov
mv -f /home/kerman/projects/GoogleTestC/mergegcov/tcprtns.c.gcov /home/kerman/projects/GoogleTestC/build/CMakeFiles/mysys.dir/master/src/library

/home/kerman/projects/GoogleTestC/mergegcov/build/mergegcov /home/kerman/projects/GoogleTestC/testmock/build/CMakeFiles/mysystest.dir/utils/memrtns.c.gcov /home/kerman/projects/GoogleTestC/build/CMakeFiles/mysys.dir/master/src/utils/memrtns.c.gcov /home/kerman/projects/GoogleTestC/mergegcov/memrtns.c.gcov
mv -f /home/kerman/projects/GoogleTestC/mergegcov/memrtns.c.gcov /home/kerman/projects/GoogleTestC/build/CMakeFiles/mysys.dir/master/src/utils

/home/kerman/projects/GoogleTestC/mergegcov/build/mergegcov /home/kerman/projects/GoogleTestC/testfake/build/CMakeFiles/mysystest.dir/programs/receipt.c.gcov /home/kerman/projects/GoogleTestC/build/CMakeFiles/mysys.dir/master/src/programs/receipt.c.gcov /home/kerman/projects/GoogleTestC/mergegcov/receipt.c.gcov
mv -f /home/kerman/projects/GoogleTestC/mergegcov/receipt.c.gcov /home/kerman/projects/GoogleTestC/build/CMakeFiles/mysys.dir/master/src/programs

cd /home/kerman/projects/GoogleTestC/build/CMakeFiles/mysys.dir/master/src
grep -r --include='*.gcov' -e '#####:' -e '*:' . | sort  > /home/kerman/projects/GoogleTestC/gcov.txt
cd /home/kerman/projects/GoogleTestC
