#!/bin/sh

# On Slurm-based clusters, use command
# $ srun -c 5 --mem 10GB --time 20:00 -- sh ../scripts/run_tests.sh 5
# otherwise just run
# $ sh ../scripts/run_tests.sh 5
# from inside your build directory

CPUS=$1

cmake --build . --target reticula_tests -j $CPUS
./reticula_tests --list-test-names-only | sed 's/,/\\,/g' | \
  parallel --halt now,fail=1 -j $CPUS -- ./reticula_tests -r compact {}
