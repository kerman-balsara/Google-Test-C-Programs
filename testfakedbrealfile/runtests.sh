stty -echo
rm -f /home/kerman/projects/GoogleTestC/testfolder/email/000002_000004_receipt >/dev/null 2>&1
rm -f /home/kerman/projects/GoogleTestC/testfolder/archive/receipt/000002_000004_receipt >/dev/null 2>&1
stty echo
if [ -z "$1" ]; then
    /home/kerman/projects/GoogleTestC/testfakedbrealfile/build/mysystest
else
    /home/kerman/projects/GoogleTestC/testfakedbrealfile/build/mysystest --gtest_filter=$1
fi