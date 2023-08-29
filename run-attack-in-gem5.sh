#!/bin/bash
source ~/.bashrc; 
# for((i=0; i<100; i++))
# do
j=0;
echo "Timeline,Value,Latency" > results.csv;

for n in 0 0 1 0 0 0 0 0 1 0 0 0 0 0 0 0 1 0 1 1 0 0 1 1 1 0 0 1 0 0 1 0 1 1 1 0 1 0 1 0 1 1 0 0 0 0 1 0 0 0 1 1 1 1 0 1 1 0 1 0 0 0 0 1 1 1 0 1 0 0 0 1 1 1 0 0 1 0 1 1 1 0 1 0 0 1 1 1 1 0 0 1 0 0 0 0 0 1 0 1
do
  j=$((j+1));
  echo -n "$j," >> results.csv;
  ./build/X86_MESI_Two_Level/gem5.opt \
    configs/example/se.py \
    --redirects /lib64=/home/grads/f/farabi/lib64 \
    --num-cpu=64 \
    --num-dirs=64 \
    --network=garnet2.0 \
    --ruby  \
    --caches \
    --l2cache \
    --l1d_size=16kB \
    --l1d_assoc=4 \
    --l2_size=2MB \
    --l2_assoc=8 \
    --num-l2cache=64 \
    --mesh-row=8 \
    --topology=Mesh_XY \
    --cmd=simulated-attack \
    --options=$n \
    --mem-size=4GB;
done; 
