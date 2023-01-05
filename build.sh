git submodule init
git submodule update
cd your-team
./bootstrap
./configure
make
cp src/sample_player ../bin/your_player
cd ..
mkdir build
cd build
sudo apt install -y cmake
cmake .. -G "Unix Makefiles"
make
cd ..
cp -r script/* bin
sudo chmod 777 bin/*
