mpirun -np 16 ./output 1 1000 > sample1000.txt
mpirun -np 3 ./output 1 10 > sample10.txt
mpirun -np 8 ./output 127 255 > sample255.txt
mpirun -np 25 ./output 1500 2500 > sample2500.txt
