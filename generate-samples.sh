mpirun -np 16 ./output 1000 > sample1000.txt
mpirun -np 3 ./output 10 > sample10.txt
mpirun -np 8 ./output 255 > sample255.txt
mpirun -np 25 ./output 2500 > sample2500.txt
