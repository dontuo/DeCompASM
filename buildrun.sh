rm a.out
g++ src/main.cpp -O3
./a.out commands.txt
cd DeComp
rm DeComp.mem
cp ../DeComp.mem .
