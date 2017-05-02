#!/usr/bin/bash

YOLO=`readlink -f .`
MODEL="tiny-yolo-voc"
INPUTDIM=416
VIDEOLIST=$YOLO/data/videos.list
QUERY=$YOLO/data/query.list

MODELPATH="$YOLO/models/$MODEL/$MODEL"".mdl"

echo "Setting network input dimensions to "$INPUTDIM"x"$INPUTDIM
python python/set_net_dims.py $MODEL $INPUTDIM

$YOLO/build/run_batch $MODELPATH $VIDEOLIST $QUERY
stty sane
