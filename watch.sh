#!/bin/bash

make

./sample.runme &

SAMPLE_PID_T=$!

echo $SAMPLE_PID_T

while true; do

  fswatch ./* -e "sample.runme" -e "build" -e "imgui.ini" -e "imnodes.ini" -e "watch.sh" -e "graph.bin" -e "shaders/*" -e "glsllog.txt" -e "textures/*" -1

  echo "Change detected. Restarting make..."

  kill -15 $SAMPLE_PID_T

  # get time created on file

  timeCreated=$(stat -f "%m" ./sample.runme)

  make

  newTimeCreated=$(stat -f "%m" ./sample.runme)

  if [ $timeCreated -eq $newTimeCreated ]; then
    echo "Must be error in code: no change in file"
    continue
  fi

  ./sample.runme &

  SAMPLE_PID_T=$!

  echo $SAMPLE_PID_T

done