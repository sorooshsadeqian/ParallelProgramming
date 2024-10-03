# ! /bin/bash

mkdir -p output
g++ 1.cpp && ./a.out > output/1.output
g++ 2.cpp && ./a.out > output/2.output
cat /proc/cpuinfo > output/cpuinfo.output
