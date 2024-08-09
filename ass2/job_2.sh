#!/bin/bash
#SBATCH --time=00:05:00
#SBATCH --account=mcs
mpirun -np 40 -mca btl ^openib ./find_passwd_mpi.x
