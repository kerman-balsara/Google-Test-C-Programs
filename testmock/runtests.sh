if [ -z "$1" ]; then
    /home/kerman/projects/GoogleTestC/testmock/build/mysystest
else
    /home/kerman/projects/GoogleTestC/testmock/build/mysystest --gtest_filter=$1
fi
