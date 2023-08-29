# Simulated Attack Code

To launch this attack, you need to use Gem5 Microarchitectural Simulator. 



## Get Gem5 Code
Clone this repository 
```
git clone https://github.com/farabimahmud/gem5-aok-poc
```
This is based on the Gem5 v20.0.0.0 release.

Or alternatively, you can download the Gem5 from the gem5 official repository - https://github.com/gem5/gem5/archive/refs/tags/v20.0.0.0.zip

## Build Gem5 
First build the simulator using the following command 
```
./xbuild.sh 
```
Alternatively, you can execute the following also directly
```
python3 -m `which scons` build/X86_MESI_Two_Level/gem5.opt -j12 PROTOCOL=MESI_Two_Level
```
The detailed process to setup Gem5 can be found in their [Official Gem5 Website](https://www.gem5.org/). 



## Compile the attack code
The attack code is given in the `simulated-attack.cpp` file 
You can compile it using the following command - 
```
g++ simulated-attack.cpp -O0 -o simulated-attack
```

Make sure to change the filename in `simulated-attack.cpp` where the results would be saved.

## Running the attack using Gem5 configuration
To run the attack in Gem5 simulator you can use the `run-attack-in-gem5.sh` file. 
It contains the current configuration for the Gem5 simulator and also specifies the attack.
We have generated 100 numbers from using python3 `random.choice` while using the random seed 42

Here is the detailed script for the gem5 script 
```
  ./build/X86_MESI_Two_Level/gem5.opt \
    configs/example/se.py \
    --redirects /lib64=/path/to/library/lib64 \
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
```

You need to update the library path to redirect the lib64 files. 

This would update the results.csv file. Each line of this .csv file contains three values. index of the array accessed, latency for accessing that address and actual bit of the secret. 

We should be able to predict based on the latency whether the prediction was correct or not. For current implementation if secret bit is 0 then the latency is expected to be lower. 

## Plotting The Result
You should be able to plot the result by using the script `plot_gem5_data.py` with trivial modifications needed from the system. 
Make sure to set the threshold accordingly. 