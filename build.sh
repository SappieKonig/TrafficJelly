cd cpp
rm -rf build
mkdir build
cd build
cmake ..
make
mv traffic_model.so ../../