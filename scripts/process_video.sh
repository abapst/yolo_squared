#!/bin/bash

YOLO=`readlink -f .`
VIDEO="$YOLO/data/cars.mp4"
INPUTDIM=416
CSV_OUT="output/cars.csv"
MODEL="tiny-yolo-voc"
QUERY="data/query.list"
VID_OUT="output/output.mp4"

MODELPATH="$YOLO/models/$MODEL/$MODEL"".mdl"

echo "Setting network input dimensions to "$INPUTDIM"x"$INPUTDIM
python python/set_net_dims.py $MODEL $INPUTDIM

echo "Processing video $VIDEO""..."
echo "VIDEO -> CSV"
$YOLO/build/run_detection $VIDEO $MODELPATH $QUERY

echo "VIDEO + CSV -> LABELED VIDEO"
$YOLO/build/run_draw_boxes $VIDEO $CSV_OUT $VID_OUT data/labels
stty sane
