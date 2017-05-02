#!/bin/bash

YOLO=`readlink -f .`
MODEL="tiny-yolo-voc"
INPUTDIM=416
ITERS=1000
QUERY=$YOLO/data/query.list
IMAGE=$YOLO/data/test.jpg

MODELPATH="$YOLO/models/$MODEL/$MODEL"".mdl"

echo "Setting network input dimensions to "$INPUTDIM"x"$INPUTDIM
python python/set_net_dims.py $MODEL $INPUTDIM

$YOLO/build/run_gpu_test $MODELPATH $IMAGE $QUERY $ITERS
stty sane
