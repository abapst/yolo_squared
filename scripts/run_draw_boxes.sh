#!/usr/bin/bash

DARKNET="/home/abbapst/darknet"
ROOT="/home/abbapst/projects/yolo_addons"
VIDNAME="cars20"

arg0="$ROOT/build/run_draw_boxes"
arg1="/project/other/dragoneye/testing/videos/""$VIDNAME"".mp4"
arg2="$ROOT/output/""$VIDNAME"".csv"
arg3="$ROOT/output.mp4"

run_cmd="$arg0 $arg1 $arg2 $arg3"
echo $run_cmd
$run_cmd
stty sane
