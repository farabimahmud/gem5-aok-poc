#!/bin/bash

python3 -m `which scons` build/X86_MESI_Two_Level/gem5.opt -j12 PROTOCOL=MESI_Two_Level
