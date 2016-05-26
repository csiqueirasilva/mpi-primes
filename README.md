# mpi-primes
Finding primes using MPI

## Install in Ubuntu with
> sudo apt-get install libcr-dev mpich2 mpich2-doc

## Compile using
> mpicc primes.c -o output

## Generating samples
> ./generate-samples.sh

## Running the program (example using 16 processes with a search range from 500 to 1000)
> mpirun -np 16 ./output 500 1000
