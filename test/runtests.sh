if [ -z "$1" ]; then
    /home/kerman/projects/GoogleTestC/test/build/mysystest
else
    /home/kerman/projects/GoogleTestC/test/build/mysystest --gtest_filter=$1
fi
