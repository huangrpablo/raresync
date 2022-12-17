#!/bin/bash
rep=({0..3})

for i in "${rep[@]}"; do
  echo "starting replica $i"
  ../example/run_with_core $i >> log${i}.txt 2 >&1 &
done
wait