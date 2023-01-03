mkdir build
cd build
cmake .. -G "Unix Makefiles"
make
cd ..
cp -r script/* bin