#!/bin/bash
rep=({0..4})

for i in "${rep[@]}"; do
  echo "starting replica $i"
  ./example/run_instance i > log${i} 2 >&1 &
done
wait