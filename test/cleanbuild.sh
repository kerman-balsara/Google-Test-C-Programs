rm -rf build
cmake -S . -G "Unix Makefiles" -B ./build
cd build
make
cd ..
