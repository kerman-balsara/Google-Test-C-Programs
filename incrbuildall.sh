./incrbuild.sh
cd test
./incrbuild.sh
cd ../testmock
./incrbuild.sh
cd ../testfake
./incrbuild.sh
cd ../testfakedbrealfile
./incrbuild.sh
cd ../mergegcov
./cleanbuild.sh
cd ../client
./cleanbuild.sh
cd ../bankserver
./cleanbuild.sh
cd ..
