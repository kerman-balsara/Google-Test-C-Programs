stty -echo
rm -f /home/kerman/projects/GoogleTestC/testfolder/email/999998_000098_receipt >/dev/null 2>&1
stty echo

if [ -z "$1" ]; then
    /home/kerman/projects/GoogleTestC/testfake/build/mysystest
else
    /home/kerman/projects/GoogleTestC/testfake/build/mysystest --gtest_filter=$1
fi
