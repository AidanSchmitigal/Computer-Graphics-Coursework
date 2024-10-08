#!/bin/bash

make

./sample.runme &

SAMPLE_PID_T=$!

echo $SAMPLE_PID_T

while true; do

  fswatch ./* -e "sample.runme" -1

  echo "Change detected. Restarting make..."

  kill -9 $SAMPLE_PID_T

  make

  ./sample.runme &

  SAMPLE_PID_T=$!

  echo $SAMPLE_PID_T

done