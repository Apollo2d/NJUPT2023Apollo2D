sudo su
mkdir build
cd build
cmake .. -G "Unix Makefiles"
make
cd..
cp script/* bin