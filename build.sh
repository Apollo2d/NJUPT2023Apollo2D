mkdir build
cd build
cmake .. -G "Unix Makefiles"
make
cd ..
cp -r script/* bin
sudo chmod 777 bin/*
